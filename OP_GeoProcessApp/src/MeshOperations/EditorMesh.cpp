#include <Precomp.h>
#include <MeshOperations/EditorMesh.h>
#include <GeoProcess/System/ResourceSystem/ResourceManager.h>
#include <GeoProcess/System/Profiling/Timer.h>

namespace GP
{

	auto vecCompare = [](VertexNode* lhs, VertexNode* rhs)
	{
		return lhs->shortestPathEstimate < rhs->shortestPathEstimate;
	};

	EditorMesh::EditorMesh(Ref<Model> model)
	{
		m_MainMesh = model->GetMesh(0);
		m_MainMesh.Name = model->GetName();

		std::vector<glm::vec3> vertices = { glm::vec3(0.0f, 0.0f, 0.0f) };
		m_Line = Line::Create(vertices);
		BuildVertices();
	}

	Ref<EditorMesh> EditorMesh::Create(Ref<Model> model)
	{
		return std::make_shared<EditorMesh>(model);
	}

	Ref<Mesh> EditorMesh::GetMainMesh()
	{
		return m_MainMesh.Mesh;
	}

	Ref<Line> EditorMesh::GetLine()
	{
		return m_Line;
	}

	std::string EditorMesh::GiveCalcMethodName()
	{
		if (m_GeodesicDistanceCalcMethod == 0)
			return "Min Heap";
		else if (m_GeodesicDistanceCalcMethod == 1)
			return "Array";
	}

	void EditorMesh::BuildVertices()
	{
		m_Vertices = m_MainMesh.Mesh->GetVertices();
		m_Normals = m_MainMesh.Mesh->GetNormals();
		m_TexCoords = m_MainMesh.Mesh->GetTexCoords();

		m_Indices = m_MainMesh.Mesh->GetIndicesVector();

		SetupNodeTable();
		SetupAdjacencyMap();

		SetupTangentBitangents(false);
		SetupArrayBuffer();
		SetupMesh();

		ComputeNxNGeodesicDistanceMatrix();
		ExportNxNGeodesicDistanceMatrix();
	}

	void EditorMesh::SetupLineVertices()
	{
		if ((m_StartIndex > -1 && m_StartIndex < m_Vertices.size()) &&
			(m_EndIndex > -1 && m_EndIndex < m_Vertices.size()) && 
			 m_EndIndex != m_StartIndex)
		{
			std::vector<glm::vec3> vertices;

			ClearNodeTable();
			ComputeGeodesicDistances(m_StartIndex);


			VertexNode currentNode = m_NodeTable[m_EndIndex];

			while (currentNode.prevIndex != -1)
			{
				vertices.insert(vertices.begin(), m_Vertices[currentNode.index]);				
				currentNode = m_NodeTable[currentNode.prevIndex];
			}

			m_Line->SetNewVertices(vertices);

		}
	}

	void EditorMesh::SetupAdjacencyMap()
	{
		for (uint32_t i = 0; i < m_Indices.size(); i += 3)
		{
			uint32_t idx1 = m_Indices[i];
			uint32_t idx2 = m_Indices[i + 1];
			uint32_t idx3 = m_Indices[i + 2];

			if (std::find(m_AdjacencyMap[idx1].begin(), m_AdjacencyMap[idx1].end(), idx2) == m_AdjacencyMap[idx1].end())
				m_AdjacencyMap[idx1].push_back(idx2);
			if (std::find(m_AdjacencyMap[idx1].begin(), m_AdjacencyMap[idx1].end(), idx3) == m_AdjacencyMap[idx1].end())
				m_AdjacencyMap[idx1].push_back(idx3);

			if (std::find(m_AdjacencyMap[idx2].begin(), m_AdjacencyMap[idx2].end(), idx1) == m_AdjacencyMap[idx2].end())
				m_AdjacencyMap[idx2].push_back(idx1);
			if (std::find(m_AdjacencyMap[idx2].begin(), m_AdjacencyMap[idx2].end(), idx3) == m_AdjacencyMap[idx2].end())
				m_AdjacencyMap[idx2].push_back(idx3);

			if (std::find(m_AdjacencyMap[idx3].begin(), m_AdjacencyMap[idx3].end(), idx1) == m_AdjacencyMap[idx3].end())
				m_AdjacencyMap[idx3].push_back(idx1);
			if (std::find(m_AdjacencyMap[idx3].begin(), m_AdjacencyMap[idx3].end(), idx2) == m_AdjacencyMap[idx3].end())
				m_AdjacencyMap[idx3].push_back(idx2);
		}
	}

	void EditorMesh::SetupNodeTable()
	{
		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			m_NodeTable[i].index = i;
			m_NodeTable[i].prevIndex = -1;
			m_NodeTable[i].shortestPathEstimate = std::numeric_limits<float>::max();
			m_NodeTable[i].visited = false;
			m_NodeTable[i].seen = false;
		}
	}

	void EditorMesh::ClearNodeTable()
	{
		for (auto& el : m_NodeTable)
		{
			el.second.prevIndex = -1;
			el.second.shortestPathEstimate = std::numeric_limits<float>::max();
			el.second.visited = false;
			el.second.seen = false;
		}
	}

	void EditorMesh::ComputeGeodesicDistances(uint32_t index)
	{
		Timer t;
		// Use min heap
		if (m_GeodesicDistanceCalcMethod == 0)
		{
			ComputeGeodesicDistancesMinHeap(index);
		}
		// Use vector
		else if (m_GeodesicDistanceCalcMethod == 1)
		{
			ComputeGeodesicDistancesVector(index);
		}

		m_CalcTime = t.ElapsedMilliseconds();
	}

	void EditorMesh::ComputeGeodesicDistancesMinHeap(uint32_t index)
	{
		// Initialize starting vertex
		m_NodeTable[index].visited = false;
		m_NodeTable[index].shortestPathEstimate = 0.0f;
		m_NodeTable[index].prevIndex = -1;
		m_NodeTable[index].index = index;
		m_NodeTable[index].seen = true;
		// Push starting vertex to the queue
		m_MinHeap.push(&m_NodeTable[index]);

		while (!m_MinHeap.empty())
		{
			VertexNode* topNode = m_MinHeap.top();
			m_MinHeap.pop();

			m_NodeTable[topNode->index].visited = true;

			for (auto el : m_AdjacencyMap[topNode->index])
			{

				// If the adjacent node has already been visited
				// do not process it
				if (m_NodeTable[el].visited)
					continue;

				// Calculate distance betwene two vertices
				glm::vec3 current, neighbor;
				current = m_Vertices[topNode->index];
				neighbor = m_Vertices[el];
				float distance = glm::distance(current, neighbor);

				// Compare distance
				if (distance + m_NodeTable[topNode->index].shortestPathEstimate < m_NodeTable[el].shortestPathEstimate)
				{
					m_NodeTable[el].shortestPathEstimate = distance + m_NodeTable[topNode->index].shortestPathEstimate;
					m_NodeTable[el].prevIndex = topNode->index;
				}

				if (!m_NodeTable[el].seen)
				{
					m_MinHeap.push(&m_NodeTable[el]);
					m_NodeTable[el].seen = true;
				}
			}
		}
	}

	void EditorMesh::ComputeGeodesicDistancesVector(uint32_t index)
	{
		// Initialize starting vertex
		m_NodeTable[index].visited = false;
		m_NodeTable[index].shortestPathEstimate = 0.0f;
		m_NodeTable[index].prevIndex = -1;
		m_NodeTable[index].index = index;

		// Push starting vertex to the queue
		m_Vector.push_back(&m_NodeTable[index]);

		while (!m_Vector.empty())
		{
			VertexNode* topNode = *std::min_element(m_Vector.begin(), m_Vector.end(), vecCompare);
			m_Vector.erase(std::remove(m_Vector.begin(), m_Vector.end(), topNode), m_Vector.end());

			m_NodeTable[topNode->index].visited = true;

			for (auto el : m_AdjacencyMap[topNode->index])
			{

				// If the adjacent node has already been visited
				// do not process it
				if (m_NodeTable[el].visited)
					continue;

				// Calculate distance betwene two vertices
				glm::vec3 current, neighbor;
				current = m_Vertices[topNode->index];
				neighbor = m_Vertices[el];
				float distance = glm::distance(current, neighbor);

				// Compare distance
				if (distance + m_NodeTable[topNode->index].shortestPathEstimate < m_NodeTable[el].shortestPathEstimate)
				{
					m_NodeTable[el].shortestPathEstimate = distance + m_NodeTable[topNode->index].shortestPathEstimate;
					m_NodeTable[el].prevIndex = topNode->index;
				}

				if (!m_NodeTable[el].seen)
				{
					m_Vector.push_back(&m_NodeTable[el]);
					m_NodeTable[el].seen = true;
				}
			}
		}
	}

	void EditorMesh::ComputeNxNGeodesicDistanceMatrix()
	{
		// Clear our node table first
		ClearNodeTable();

		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			std::vector<float> row;
			ComputeGeodesicDistances(i);
			for (uint32_t j = 0; j < m_Vertices.size(); j++)
			{
				row.push_back(m_NodeTable[j].shortestPathEstimate);
			}

			m_NxNGeodesicDistanceMatrix.push_back(row);
			ClearNodeTable();
		}
		

		// Open output file
		//std::filesystem::path outputPath = ResourceManager::GetOutputDirectory() / "output.out";

		//std::streamoff out(outputPath)
	}

	void EditorMesh::ExportNxNGeodesicDistanceMatrix()
	{
		// Open output file
		std::filesystem::path outputPath = ResourceManager::GetOutputDirectory() / std::string( "M_for_" + m_MainMesh.Name + ".out");

		std::ofstream out(outputPath, std::ios::out);

		for (uint32_t i = 0; i < m_NxNGeodesicDistanceMatrix.size(); i++)
		{
			for (uint32_t j = 0; j < m_NxNGeodesicDistanceMatrix[i].size(); j++)
			{
				out << m_NxNGeodesicDistanceMatrix[i][j];

				if (j < m_NxNGeodesicDistanceMatrix[i].size() - 1)
					out << " ";
			}

			if (i < m_NxNGeodesicDistanceMatrix.size() - 1)
				out << "\n";
		}

		out.close();
	}
}