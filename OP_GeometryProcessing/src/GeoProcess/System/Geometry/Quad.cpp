#include <Precomp.h>
#include <GeoProcess/System/Geometry/Quad.h>

namespace GP
{
	Quad::Quad()
	{
		BuildVertices();
	}

	Ref<Quad> Quad::Create()
	{
		return std::make_shared<Quad>();
	}


	void Quad::BuildVertices()
	{


		// Clear arrays for safety
		ClearArrays();

		// Fill in vertices, normals and indices
		m_Vertices.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
		m_Normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 1.0f));

		m_Vertices.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
		m_Normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 0.0f));

		m_Vertices.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
		m_Normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 0.0f));

		m_Vertices.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
		m_Normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 1.0f));

		// set indices by constructing triangles
		m_Indices.push_back(0); m_Indices.push_back(1); m_Indices.push_back(2);
		m_Indices.push_back(0); m_Indices.push_back(2); m_Indices.push_back(3);


		SetupTangentBitangents(false);
		SetupArrayBuffer();
		SetupMesh();
	}
}
