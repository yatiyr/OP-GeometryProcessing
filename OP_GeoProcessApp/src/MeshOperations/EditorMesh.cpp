#include <Precomp.h>
#include <MeshOperations/EditorMesh.h>
#include <GeoProcess/System/ResourceSystem/ResourceManager.h>
#include <GeoProcess/System/Profiling/Timer.h>
#include <GeoProcess/System/Geometry/Icosphere.h>

#include <glad/glad.h>

#include <GeoProcess/System/RenderSystem/RenderCommand.h>

namespace GP
{

	static glm::vec3 Rgb2Hsv(glm::vec3 rgbColor)
	{

		double cmax = std::max(rgbColor.r, std::max(rgbColor.g, rgbColor.b));
		double cmin = std::min(rgbColor.r, std::min(rgbColor.g, rgbColor.b));
		double diff = cmax - cmin;
		double h = -1, s = -1;

		if (cmax == cmin)
			h = 0;

		else if (cmax == rgbColor.r)
			h = std::fmod(60.0f * ((rgbColor.g - rgbColor.b) / diff) + 360.0f, 360);

		else if (cmax == rgbColor.g)
			h = std::fmod(60.0f * ((rgbColor.b - rgbColor.r) / diff) + 120.0f, 360);

		else if (cmax == rgbColor.b)
			h = std::fmod(60.0f * ((rgbColor.r - rgbColor.g) / diff) + 240.0f, 360);

		if (cmax == 0)
			s = 0;
		else
			s = (diff / cmax) * 100.0f;

		double v = cmax * 100;

		return glm::vec3(h, s, v);
	}

	static glm::vec3 Hsv2Rgb(glm::vec3 hsvColor)
	{
		float fC = hsvColor.z * hsvColor.y;
		float fHprime = std::fmod(hsvColor.x / 60.0f, 6);
		float fX = fC * (1 - std::fabs(std::fmod(fHprime, 2) - 1));
		float fM = hsvColor.z - fC;

		glm::vec3 resultRgb;

		if (0 <= fHprime && fHprime < 1)
		{
			resultRgb = glm::vec3(fC, fX, 0.0f);
		}
		else if (1 <= fHprime && fHprime < 2)
		{
			resultRgb = glm::vec3(fX, fC, 0.0f);
		}
		else if (2 <= fHprime && fHprime < 3)
		{
			resultRgb = glm::vec3(0.0f, fC, fX);
		}
		else if (3 <= fHprime && fHprime < 4)
		{
			resultRgb = glm::vec3(0.0f, fX, fC);
		}
		else if (4 <= fHprime && fHprime < 5)
		{
			resultRgb = glm::vec3(fX, 0.0f, fC);
		}
		else if (5 <= fHprime && fHprime < 6)
		{
			resultRgb = glm::vec3(fC, 0.0f, fX);
		}
		else
		{
			resultRgb = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		resultRgb += glm::vec3(fM, fM, fM);
		
		return resultRgb;
	}

	static glm::vec3 GiveGradientColorBetweenRedAndBlue(float t)
	{
		// A color between red and blue (passes from green in the middle)
		glm::vec3 hsvColor = glm::vec3(std::min(t, 1.0f) * 228.0f, 1.0, 0.6f);

		return Hsv2Rgb(hsvColor);		
	}

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

		m_Sphere = Icosphere::Create(0.8f, 1, false);
		BuildVertices();

		m_CoreSize = std::thread::hardware_concurrency();
		m_Count = 0;
	}

	EditorMesh::EditorMesh(std::string name, const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices)
	{
		m_MainMesh.Name = name;

		std::vector<glm::vec3> lineverts = { glm::vec3(0.0f, 0.0f, 0.0f) };
		m_Line = Line::Create(lineverts);

		m_Sphere = Icosphere::Create(0.8f, 1, false);
		m_Vertices = vertices;
		m_Indices = indices;
		BuildVerticesNoMesh();

		m_CoreSize = std::thread::hardware_concurrency();
		m_Count = 0;
	}

	Ref<EditorMesh> EditorMesh::Create(Ref<Model> model)
	{
		return std::make_shared<EditorMesh>(model);
	}

	Ref<EditorMesh> EditorMesh::Create(std::string name, const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices)
	{
		return std::make_shared<EditorMesh>(name, vertices, indices);
	}

	Ref<Mesh> EditorMesh::GetMainMesh()
	{
		return m_MainMesh.Mesh;
	}

	Ref<Line> EditorMesh::GetLine()
	{
		return m_Line;
	}

	Ref<Icosphere> EditorMesh::GetSphere()
	{
		return m_Sphere;
	}

	std::string EditorMesh::GiveCalcMethodName()
	{
		if (m_GeodesicDistanceCalcMethod == 0)
			return "Min Heap";
		else if (m_GeodesicDistanceCalcMethod == 1)
			return "Array";
	}

	std::string EditorMesh::GiveRenderMethodName()
	{
		if (m_RenderSpecs.renderMode == RENDERMODE::AGD)
		{
			return "AGD";
		}
		else if (m_RenderSpecs.renderMode == RENDERMODE::FLAT)
		{
			return "Flat";
		}
		else if (m_RenderSpecs.renderMode == RENDERMODE::GC)
		{
			return "GC";
		}
		else if (m_RenderSpecs.renderMode == RENDERMODE::QUALITY)
		{
			return "Triangle Quality";
		}
		else if (m_RenderSpecs.renderMode == RENDERMODE::SMOOTH)
		{
			return "Smooth";
		}

		return std::string();
	}

	void EditorMesh::BuildVertices()
	{

		// Get Vertices normals, tex coordinates and indices 
		// from the main mesh
		m_Vertices = m_MainMesh.Mesh->GetVertices();
		m_Normals = m_MainMesh.Mesh->GetNormals();
		m_TexCoords = m_MainMesh.Mesh->GetTexCoords();
		m_Indices = m_MainMesh.Mesh->GetIndicesVector();

		// We first setup the triangles each triangle will
		// hold 3 indices for vertices in CCW direction and
		// the indices follow the order of m_Indices
		SetupTriangles();

		// We also need a flat shaded version because for quality
		// coloring, we need to color individual triangles, I might
		// change this to something better in the future If I can
		// find one but this is the solution now
		SetupFlatElements();

		// Node table is used for keeping track of vertices while running
		// dijkstra's shortest path algorithm, it is used for geodesic distances
		SetupNodeTable();
		SetupNodeTableExport();

		// Adjacency map is an unordered map (hash table) which holds the
		// indices of vertices that are neighbors to a specific vertex
		// it is used for coloring computations and also geodesic distances
		SetupAdjacencyMap();

		// Tangents and bitangents are calculated from normals, texture coordinates
		// and vertex positions, in our case we might not need for this because we
		// do not deal with textures in geometry processing course
		SetupTangentBitangents(false);

		
		// We compute colors of each vertices using Average Geodesic Distances and
		// gaussian curvature. We also compute colors for each triangle's quality
		// all these values for Average Geodesic Distance, Gaussian Curvature and
		// Triangle's quality are between 0 and 1 and the colors assigned to them
		// are between red and blue (red - green - blue). For getting color gradient
		// I have used hsv color space instead of rgb.
		CalculateColors();

		// After calculating the colors, we setup array buffer for drawing with color
		// there are separate vertex array objects and vertex buffer objects because
		// I did not want to fill the buffers everytime I change a drawing mode
		// Quality array buffer uses flat coloring because we want to color each
		// triangle
		SetupArrayBufferForColoring(m_AGDArrayBuffer, m_AverageGeodesicDistanceColors);
		SetupArrayBufferForColoring(m_GCArrayBuffer, m_GaussianCurvatureColors);
		SetupArrayBufferForFlatColoring(m_QualityArrayBuffer, m_QualityColors);

		// We register array buffers to the mesh for coloring
		SetupMeshForColoring(m_AGDVertexArray, m_AGDVertexBuffer,
							 m_AGDArrayBuffer, m_AGDIndexBuffer);

		SetupMeshForColoring(m_GCVertexArray, m_GCVertexBuffer,
							 m_GCArrayBuffer, m_GCIndexBuffer);

		SetupMeshForFlatColoring(m_QualityVertexArray, m_QualityVertexBuffer,
								 m_QualityArrayBuffer, m_QualityIndexBuffer);

		// We setup and register array buffers for drawing normally
		SetupArrayBuffer();
		SetupMesh();

		 // These will be separated as different functions
		//ComputeNxNGeodesicDistanceMatrix();
		//ExportNxNGeodesicDistanceMatrix();
	}

	// This function is mostly the same with build vertices. However
	// the difference is that we use this function if we construct our
	// mesh from set of vertices and indices we get from a database
	void EditorMesh::BuildVerticesNoMesh()
	{

		// We first setup the triangles each triangle will
		// hold 3 indices for vertices in CCW direction and
		// the indices follow the order of m_Indices
		SetupTriangles();

		// Calculate smooth normals
		CalculateSmoothNormals();

		// We also need a flat shaded version because for quality
		// coloring, we need to color individual triangles, I might
		// change this to something better in the future If I can
		// find one but this is the solution now
		SetupFlatElements();

		// Node table is used for keeping track of vertices while running
		// dijkstra's shortest path algorithm, it is used for geodesic distances
		SetupNodeTable();
		SetupNodeTableExport();

		// Adjacency map is an unordered map (hash table) which holds the
		// indices of vertices that are neighbors to a specific vertex
		// it is used for coloring computations and also geodesic distances
		SetupAdjacencyMap();

		// Tangents and bitangents are calculated from normals, texture coordinates
		// and vertex positions, in our case we might not need for this because we
		// do not deal with textures in geometry processing course
		SetupTangentBitangents(false);


		// We compute colors of each vertices using Average Geodesic Distances and
		// gaussian curvature. We also compute colors for each triangle's quality
		// all these values for Average Geodesic Distance, Gaussian Curvature and
		// Triangle's quality are between 0 and 1 and the colors assigned to them
		// are between red and blue (red - green - blue). For getting color gradient
		// I have used hsv color space instead of rgb.
		CalculateColors();

		// After calculating the colors, we setup array buffer for drawing with color
		// there are separate vertex array objects and vertex buffer objects because
		// I did not want to fill the buffers everytime I change a drawing mode
		// Quality array buffer uses flat coloring because we want to color each
		// triangle
		SetupArrayBufferForColoring(m_AGDArrayBuffer, m_AverageGeodesicDistanceColors);
		SetupArrayBufferForColoring(m_GCArrayBuffer, m_GaussianCurvatureColors);
		SetupArrayBufferForFlatColoring(m_QualityArrayBuffer, m_QualityColors);

		// We register array buffers to the mesh for coloring
		SetupMeshForColoring(m_AGDVertexArray, m_AGDVertexBuffer,
			m_AGDArrayBuffer, m_AGDIndexBuffer);

		SetupMeshForColoring(m_GCVertexArray, m_GCVertexBuffer,
			m_GCArrayBuffer, m_GCIndexBuffer);

		SetupMeshForFlatColoring(m_QualityVertexArray, m_QualityVertexBuffer,
			m_QualityArrayBuffer, m_QualityIndexBuffer);

		// We setup and register array buffers for drawing normally
		SetupArrayBuffer();
		SetupMesh();

		// These will be separated as different functions
	   //ComputeNxNGeodesicDistanceMatrix();
	   //ExportNxNGeodesicDistanceMatrix();
	}

	void EditorMesh::SetupTriangles()
	{
		if (m_Triangles.size() > 0)
			m_Triangles.clear();

		for (uint32_t i = 0; i < m_Indices.size(); i += 3)
		{
			Triangle triangle;
			triangle.idx1 = m_Indices[i];
			triangle.idx2 = m_Indices[i + 1];
			triangle.idx3 = m_Indices[i + 2];

			m_Triangles.push_back(triangle);
		}
	}

	void EditorMesh::SmoothingFunction()
	{
		std::unordered_map<uint32_t,glm::vec3> displacementMap;

		for (uint32_t i = 0; i < m_Indices.size(); i++)
			displacementMap[i] = glm::vec3(0.0f, 0.0f, 0.0f);

		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			glm::vec3 center = m_Vertices[i];

			for (auto neighbor : m_AdjacencyMap[i])
			{
				glm::vec3 neighborPos = m_Vertices[neighbor];
				glm::vec3 dir = glm::normalize(center - neighborPos);
				displacementMap[neighbor] += dir * m_SmootingFactor;
			}
		}

		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			m_Vertices[i] += displacementMap[i];
		}


		SetupTriangles();
		SetupFlatElements();
		SetupNodeTable();
		SetupNodeTableExport();
		SetupAdjacencyMap();
		SetupTangentBitangents(false);
		CalculateColors();
		SetupArrayBufferForColoring(m_AGDArrayBuffer, m_AverageGeodesicDistanceColors);
		SetupArrayBufferForColoring(m_GCArrayBuffer, m_GaussianCurvatureColors);
		SetupArrayBufferForFlatColoring(m_QualityArrayBuffer, m_QualityColors);
		SetupMeshForColoring(m_AGDVertexArray, m_AGDVertexBuffer,
			m_AGDArrayBuffer, m_AGDIndexBuffer);
		SetupMeshForColoring(m_GCVertexArray, m_GCVertexBuffer,
			m_GCArrayBuffer, m_GCIndexBuffer);
		SetupMeshForFlatColoring(m_QualityVertexArray, m_QualityVertexBuffer,
			m_QualityArrayBuffer, m_QualityIndexBuffer);
		SetupArrayBuffer();
		SetupMesh();

	}

	std::future<void> EditorMesh::ExportGDM()
	{
		return std::async(std::launch::async, [=]()
			{
				ComputeNxNGeodesicDistanceMatrix();
				ExportNxNGeodesicDistanceMatrix();
			}
		);
	}

	void EditorMesh::CalculateSmoothNormals()
	{
		if (!m_Normals.empty())
			m_Normals.clear();

		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			std::vector<Triangle> triangles = GiveTrianglesWithVertex(i);
			glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);

			for (auto& triangle : triangles)
				normal += triangle.GiveNormal(m_Vertices);

			normal = glm::normalize(normal);

			m_Normals.push_back(normal);
		}
	}

	std::vector<Triangle> EditorMesh::GiveTrianglesWithVertex(uint32_t index)
	{
		std::vector<Triangle> result;

		uint32_t counter = 0;

		for (auto& triangle : m_Triangles)
		{
			if (triangle.idx1 == index ||
				triangle.idx2 == index ||
				triangle.idx3 == index)
			{
				result.push_back(triangle);
				counter++;
			}
				
			if (counter >= m_AdjacencyMap[index].size())
				break;			
		}

		return result;
	}

	std::vector<uint32_t> EditorMesh::SampleNPoints(uint32_t sampleCount)
	{
		std::vector<uint32_t> result;
		std::set<uint32_t> sampleSet;

		if (sampleCount > m_Vertices.size())
			sampleCount = m_Vertices.size() - 1;

		std::srand(std::time(0));
		int lb = 0, ub = m_Vertices.size() - 1;
		sampleSet.insert(rand() % (ub - lb + 1) + lb);

		//sampleSet.insert(0);

		for (uint32_t i = 0; i < sampleCount; i++)
		{
			uint32_t maxDistIndex;
			float maxDistance = 0.0f;


			for (uint32_t j = 0; j < m_Vertices.size(); j++)
			{

				uint32_t closestDistIndex;
				float closest = std::numeric_limits<float>::max();

				if (sampleSet.find(j) == sampleSet.end())
				{
					for (auto sample : sampleSet)
					{
						float dist =  glm::length(m_Vertices[sample] - m_Vertices[j]); //  //  GiveGeodesicDistanceBetweenVertices(sample, j); //

						if (dist < closest)
						{
							closestDistIndex = sample;
							closest = dist;
						}

					}

					if (closest > maxDistance)
					{
						maxDistIndex = j;
						maxDistance = closest;
					}
					
				}
			}

			GP_TRACE("Finished: {0}", (float)i / (float)sampleCount);
			GP_FLUSH_TRACE();

			if(maxDistance != 0.0f)
				sampleSet.insert(maxDistIndex);

			maxDistance = 0.0f;


		}

		for (auto sample : sampleSet)
			result.push_back(sample);

		return result;

	}

	float EditorMesh::GiveGeodesicDistanceBetweenVertices(uint32_t idx1, uint32_t idx2)
	{
		ClearNodeTable();
		ComputeGeodesicDistancesMinHeap(idx1);
		
		return m_NodeTable[idx2].shortestPathEstimate;
	}

	glm::vec3 EditorMesh::GetVertex(uint32_t id)
	{
		return m_Vertices[id];
	}

	// I don't think that I need tangents, bitangents and texture coordinates
	// for flat shading, it is mainly for coloring individual triangles
	void EditorMesh::SetupFlatElements()
	{
		if (!m_FlatShadeVertices.empty())
			m_FlatShadeVertices.clear();

		if (!m_FlatShadeNormals.empty())
			m_FlatShadeNormals.clear();

		if (!m_FlatShadeIndices.empty())
			m_FlatShadeIndices.clear();

		for (uint32_t i = 0; i < m_Indices.size(); i++)
			m_FlatShadeVertices.push_back(m_Vertices[m_Indices[i]]);

		for (uint32_t i = 0; i < m_FlatShadeVertices.size(); i++)
			m_FlatShadeIndices.push_back(i);

		for (uint32_t i = 0; i < m_FlatShadeVertices.size(); i += 3)
		{
			glm::vec3 v1 = m_FlatShadeVertices[i];
			glm::vec3 v2 = m_FlatShadeVertices[i + 1];
			glm::vec3 v3 = m_FlatShadeVertices[i + 2];

			glm::vec3 normal = ComputeFaceNormal(v1, v2, v3);

			m_FlatShadeNormals.push_back(normal);
			m_FlatShadeNormals.push_back(normal);
			m_FlatShadeNormals.push_back(normal);
		}

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
				glm::vec3 vertNormal = m_Normals[currentNode.index];
				glm::vec3 lineVertex = m_Vertices[currentNode.index] + vertNormal * m_RenderSpecs.lineDisplacement;
				vertices.insert(vertices.begin(), lineVertex);				
				currentNode = m_NodeTable[currentNode.prevIndex];
			}

			// last vertex
			glm::vec3 vertNormal = m_Normals[currentNode.index];
			glm::vec3 lineVertex = m_Vertices[currentNode.index] + vertNormal * m_RenderSpecs.lineDisplacement;
			vertices.insert(vertices.begin(), lineVertex);
			currentNode = m_NodeTable[currentNode.prevIndex];

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

	void EditorMesh::SetupNodeTableExport()
	{
		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			m_NodeTableExport[i].index = i;
			m_NodeTableExport[i].prevIndex = -1;
			m_NodeTableExport[i].shortestPathEstimate = std::numeric_limits<float>::max();
			m_NodeTableExport[i].visited = false;
			m_NodeTableExport[i].seen = false;
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

	void EditorMesh::ClearNodeTableExport()
	{
		for (auto& el : m_NodeTableExport)
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

	void EditorMesh::ComputeGeodesicDistancesMinHeapExport(uint32_t index)
	{
		// Initialize starting vertex
		m_NodeTableExport[index].visited = false;
		m_NodeTableExport[index].shortestPathEstimate = 0.0f;
		m_NodeTableExport[index].prevIndex = -1;
		m_NodeTableExport[index].index = index;
		m_NodeTableExport[index].seen = true;
		// Push starting vertex to the queue
		m_MinHeapExport.push(&m_NodeTableExport[index]);

		while (!m_MinHeapExport.empty())
		{
			VertexNode* topNode = m_MinHeapExport.top();
			m_MinHeapExport.pop();

			m_NodeTableExport[topNode->index].visited = true;

			for (auto el : m_AdjacencyMap[topNode->index])
			{

				// If the adjacent node has already been visited
				// do not process it
				if (m_NodeTableExport[el].visited)
					continue;

				// Calculate distance betwene two vertices
				glm::vec3 current, neighbor;
				current = m_Vertices[topNode->index];
				neighbor = m_Vertices[el];
				float distance = glm::distance(current, neighbor);

				// Compare distance
				if (distance + m_NodeTableExport[topNode->index].shortestPathEstimate < m_NodeTableExport[el].shortestPathEstimate)
				{
					m_NodeTableExport[el].shortestPathEstimate = distance + m_NodeTableExport[topNode->index].shortestPathEstimate;
					m_NodeTableExport[el].prevIndex = topNode->index;
				}

				if (!m_NodeTableExport[el].seen)
				{
					m_MinHeapExport.push(&m_NodeTableExport[el]);
					m_NodeTableExport[el].seen = true;
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

	void EditorMesh::Draw(Ref<Shader> mainShader,
						  Ref<Shader> colorShader,
						  Ref<Shader> singleColorShader,
						  Ref<EnvironmentMap> envMap,
						  uint32_t ditheringTex) const
	{
		if (m_RenderSpecs.fill)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			switch (m_RenderSpecs.renderMode)
			{
				case(RENDERMODE::AGD):
					colorShader->Bind();
					colorShader->SetFloat(0, m_RenderSpecs.roughness);
					colorShader->SetFloat(1, m_RenderSpecs.metalness);
					envMap->BindIrradianceMap(0);
					envMap->BindPrefilterMap(1);
					envMap->BindBrdfLUT(2);
					glBindTextureUnit(3, ditheringTex);
					RenderCommand::DrawIndexedBinded(m_AGDVertexArray, m_Indices.size());
					break;
				case(RENDERMODE::FLAT):
					mainShader->Bind();
					mainShader->SetFloat(0, m_RenderSpecs.roughness);
					mainShader->SetFloat(1, m_RenderSpecs.metalness);
					mainShader->SetFloat3(2, m_RenderSpecs.albedo);
					envMap->BindIrradianceMap(0);
					envMap->BindPrefilterMap(1);
					envMap->BindBrdfLUT(2);
					glBindTextureUnit(3, ditheringTex);
					RenderCommand::DrawIndexedBinded(m_QualityVertexArray, m_FlatShadeIndices.size());
					break;
				case(RENDERMODE::GC):
					colorShader->Bind();
					colorShader->SetFloat(0, m_RenderSpecs.roughness);
					colorShader->SetFloat(1, m_RenderSpecs.metalness);
					envMap->BindIrradianceMap(0);
					envMap->BindPrefilterMap(1);
					envMap->BindBrdfLUT(2);
					glBindTextureUnit(3, ditheringTex);
					RenderCommand::DrawIndexedBinded(m_GCVertexArray, m_Indices.size());
					break;
				case(RENDERMODE::QUALITY):
					colorShader->Bind();
					colorShader->SetFloat(0, m_RenderSpecs.roughness);
					colorShader->SetFloat(1, m_RenderSpecs.metalness);
					envMap->BindIrradianceMap(0);
					envMap->BindPrefilterMap(1);
					envMap->BindBrdfLUT(2);
					glBindTextureUnit(3, ditheringTex);
					RenderCommand::DrawIndexedBinded(m_QualityVertexArray, m_FlatShadeIndices.size());
					break;
				case(RENDERMODE::SMOOTH):
					mainShader->Bind();
					mainShader->SetFloat(0, m_RenderSpecs.roughness);
					mainShader->SetFloat(1, m_RenderSpecs.metalness);
					mainShader->SetFloat3(2, m_RenderSpecs.albedo);
					envMap->BindIrradianceMap(0);
					envMap->BindPrefilterMap(1);
					envMap->BindBrdfLUT(2);
					glBindTextureUnit(3, ditheringTex);
					RenderCommand::DrawIndexedBinded(m_VertexArray, m_Indices.size());
					break;
			}
		}

		if (m_RenderSpecs.line)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			singleColorShader->Bind();
			singleColorShader->SetFloat4(0, m_RenderSpecs.lineColor);
			RenderCommand::DrawIndexedBinded(m_VertexArray, m_Indices.size());
		}

		if (m_RenderSpecs.point)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			singleColorShader->Bind();
			singleColorShader->SetFloat4(0, m_RenderSpecs.pointColor);
			RenderCommand::DrawIndexedBinded(m_VertexArray, m_Indices.size());
		}

		if (m_ShowLine)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glLineWidth(5.0f);
			singleColorShader->Bind();
			singleColorShader->SetFloat4(0, m_LineColor);
			m_Line->Draw();
		}

		glLineWidth(1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}

	void EditorMesh::CalculateGaussianCurvatureColors()
	{
		if (!m_GaussianCurvatureColors.empty())
			m_GaussianCurvatureColors.clear();

		std::vector<float> gaussianCurvatureNumbers;

		float maxCurvature = 0.0f;

		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			std::vector<Triangle> triangles = GiveTrianglesWithVertex(i);

			float totalCurvature = 0.0f;

			for (auto triangle : triangles)
			{
				glm::vec3 v1, v2, v3;

				v1 = m_Vertices[i];

				if (triangle.idx1 == i)
				{
					v2 = m_Vertices[triangle.idx2];
					v3 = m_Vertices[triangle.idx3];
				}
				else if (triangle.idx2 == i)
				{
					v2 = m_Vertices[triangle.idx1];
					v3 = m_Vertices[triangle.idx3];
				}
				else if (triangle.idx3 == i)
				{
					v2 = m_Vertices[triangle.idx1];
					v3 = m_Vertices[triangle.idx2];
				}

				glm::vec3 ed1 = normalize(v2 - v1);
				glm::vec3 ed2 = normalize(v3 - v1);

				float cos = glm::dot(ed1, ed2);

				totalCurvature += 2 * glm::pi<float>() - cos;
			}


			gaussianCurvatureNumbers.push_back(totalCurvature);

			if (totalCurvature > maxCurvature)
				maxCurvature = totalCurvature;
		}

		// color interpolation
		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			m_GaussianCurvatureColors.push_back(GiveGradientColorBetweenRedAndBlue(gaussianCurvatureNumbers[i] / maxCurvature));
		}

	}

	void EditorMesh::CalculateAGDColors()
	{

		m_AverageGeodesicDistanceColors.clear();
		m_SamplePoints.clear();
		m_SamplePoints = SampleNPoints(5);


		std::vector<float> avgDistances;
		avgDistances.resize(m_Vertices.size(), 0.0f);


		for (uint32_t i = 0; i < m_SamplePoints.size(); i++)
		{
			ClearNodeTable();
			ComputeGeodesicDistances(m_SamplePoints[i]);

			float distanceSum = 0.0f;
			float maxDistance = 0.0f;

			for (uint32_t j = 0; j < m_Vertices.size(); j++)
			{
				avgDistances[j] += m_NodeTable[j].shortestPathEstimate;
			}
		}

		float maxDistance = 0.0f;
		for (uint32_t i = 0; i < avgDistances.size(); i++)
		{
			avgDistances[i] /= m_SamplePoints.size();

			if (avgDistances[i] > maxDistance)
				maxDistance = avgDistances[i];
		}

		for (uint32_t i = 0; i < avgDistances.size(); i++)
		{
			m_AverageGeodesicDistanceColors.push_back(GiveGradientColorBetweenRedAndBlue(avgDistances[i] / maxDistance));
		}

	}

	void EditorMesh::CalculateQualityColors()
	{

		m_QualityColors.clear();


		std::vector<float> qualityVector;

		float maxQuality = 0.0f;
		
		for (uint32_t i = 0; i < m_Triangles.size(); i++)
		{
			if (i == 5)
				std::cout << "i = 5" << std::endl;
			float quality = m_Triangles[i].CalculateQuality(m_Vertices);
			if (quality > maxQuality)
				maxQuality = quality;

			qualityVector.push_back(quality);
			qualityVector.push_back(quality);
			qualityVector.push_back(quality);
		}

		for (uint32_t i = 0; i < qualityVector.size(); i++)
		{
			m_QualityColors.push_back(GiveGradientColorBetweenRedAndBlue(qualityVector[i] / maxQuality));
		}
	}

	void EditorMesh::CalculateColors()
	{
		CalculateGaussianCurvatureColors();
		CalculateAGDColors();
		CalculateQualityColors();
	}

	void EditorMesh::SetupColoringBuffers()
	{
	}

	void EditorMesh::SetupArrayBufferForColoring(std::vector<ColoringVertex>& coloringArrayBuffer, 
		                                         const std::vector<glm::vec3>& colors)
	{
		coloringArrayBuffer.clear();

		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			// Coloring vertex this time
			ColoringVertex vert;
			vert.Pos = m_Vertices[i];
			vert.Normal = m_Normals[i];
			vert.Color = colors[i];

			coloringArrayBuffer.push_back(vert);
		}
	}

	void EditorMesh::SetupArrayBufferForFlatColoring(std::vector<ColoringVertex>& coloringArrayBuffer,
													 const std::vector<glm::vec3>& colors)
	{
		coloringArrayBuffer.clear();

		for (uint32_t i = 0; i < m_FlatShadeVertices.size(); i++)
		{
			ColoringVertex vert;
			vert.Pos = m_FlatShadeVertices[i];
			vert.Normal = m_FlatShadeNormals[i];
			vert.Color = colors[i];

			coloringArrayBuffer.push_back(vert);
		}
		
	}

	void EditorMesh::SetupMeshForColoring(Ref<VertexArray>& coloringVertexArray,
										  Ref<VertexBuffer>& coloringVertexBuffer,
										  std::vector<ColoringVertex>& coloringArrayBuffer,
										  Ref<IndexBuffer>& coloringIndexBuffer)
	{
		//if (coloringVertexArray.get())
			// delete coloringVertexArray.get();

		coloringVertexArray = VertexArray::Create();

		coloringVertexBuffer = VertexBuffer::Create(&coloringArrayBuffer[0], coloringArrayBuffer.size() * sizeof(ColoringVertex));
		coloringVertexBuffer->SetLayout(
			{
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float3, "a_Color"}
			}
		);

		coloringVertexArray->AddVertexBuffer(coloringVertexBuffer);
		coloringIndexBuffer = IndexBuffer::Create(&m_Indices[0], m_Indices.size());
		coloringVertexArray->SetIndexBuffer(coloringIndexBuffer);
	}

	void EditorMesh::SetupMeshForFlatColoring(Ref<VertexArray>& coloringVertexArray,
											  Ref<VertexBuffer>& coloringVertexBuffer,
											  std::vector<ColoringVertex>& coloringArrayBuffer,
											  Ref<IndexBuffer>& coloringIndexBuffer)
	{
		//if (coloringVertexArray.get())
			//delete coloringVertexArray.get();

		coloringVertexArray = VertexArray::Create();

		coloringVertexBuffer = VertexBuffer::Create(&coloringArrayBuffer[0], coloringArrayBuffer.size() * sizeof(ColoringVertex));
		coloringVertexBuffer->SetLayout(
			{
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float3, "a_Color"}
			}
		);

		coloringVertexArray->AddVertexBuffer(coloringVertexBuffer);
		coloringIndexBuffer = IndexBuffer::Create(&m_FlatShadeIndices[0], m_FlatShadeIndices.size());
		coloringVertexArray->SetIndexBuffer(coloringIndexBuffer);
	}



	void EditorMesh::ComputeNxNGeodesicDistanceMatrix()
	{
		// Clear our node table first
		ClearNodeTableExport();

		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			std::vector<float> row;
			ComputeGeodesicDistancesMinHeapExport(i);
			for (uint32_t j = 0; j < m_Vertices.size(); j++)
			{
				row.push_back(m_NodeTableExport[j].shortestPathEstimate);
			}

			m_NxNGeodesicDistanceMatrix.push_back(row);
			ClearNodeTableExport();
		}
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