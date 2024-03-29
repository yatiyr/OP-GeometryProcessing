#pragma once

#define MAX_BONE_INFLUENCE 4

#include <GeoProcess/System/RenderSystem/VertexArray.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <unordered_map>


namespace GP
{
	struct BoneIds { int IDs[MAX_BONE_INFLUENCE]; };
	struct BoneWeights { float Weights[MAX_BONE_INFLUENCE]; };
	struct BoneInfo
	{
		// Index in final bone matrices
		int id;

		// matrix to transform vertex from model space to bone space
		glm::mat4 offset;
	};

	struct Vertex
	{
		glm::vec3 Pos;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;

		int BoneIds[MAX_BONE_INFLUENCE];
		float BoneWeights[MAX_BONE_INFLUENCE];
	};

	class Mesh
	{
	public:
		Mesh(bool smooth);
		Mesh();
		Mesh(aiMesh* mesh, const aiScene* scene, aiNode* currentNode, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCounter);
		~Mesh();

		virtual void BuildVertices() {}

		static Ref<Mesh> Create(aiMesh* mesh, const aiScene* scene, aiNode* currentNode, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCounter);

		virtual void SetupArrayBuffer();
		virtual void SetupMesh();

		virtual void SetupTangentBitangents(bool calculateHandedness);
		void AddIndices(uint32_t i1, uint32_t i2, uint32_t i3);

		void ClearArrays();

		uint32_t GetVertexCount()    const;
		uint32_t GetNormalCount()    const;
		uint32_t GetBitangentCount() const;
		uint32_t GetTangentCount()   const;
		uint32_t GetTexCoordCount()  const;
		uint32_t GetIndexCount()     const;
		uint32_t GetTriangleCount()  const;
		uint32_t GetVertexSize()     const;
		uint32_t GetNormalSize()     const;
		uint32_t GetTangentSize()    const;
		uint32_t GetBitangentSize()  const;
		uint32_t GetTexCoordSize()   const;
		uint32_t GetIndexSize()      const;

		const std::vector<glm::vec3> GetVertices()   const;
		const std::vector<glm::vec3> GetNormals()    const;
		const std::vector<glm::vec3> GetTangents()   const;
		const std::vector<glm::vec3> GetBitangents() const;
		const std::vector<glm::vec2> GetTexCoords() const;

		const uint32_t* GetIndices() const;
		const std::vector<uint32_t> GetIndicesVector() const;

		virtual void Draw() const;
		void SetSmooth(bool smooth);

	protected:
		std::vector<glm::vec3> CalcTangentBitangents(glm::vec3 pos1,
			glm::vec3 pos2,
			glm::vec3 pos3,
			glm::vec2 uv1,
			glm::vec2 uv2,
			glm::vec2 uv3);

		glm::vec3 ComputeFaceNormal(const glm::vec3& v1,
			const glm::vec3& v2,
			const glm::vec3& v3);

		std::vector<glm::vec3> m_Vertices;
		std::vector<glm::vec3> m_Normals;
		std::vector<glm::vec3> m_Tangents;
		std::vector<glm::vec3> m_Bitangents;
		std::vector<glm::vec2> m_TexCoords;

		// Bone related stuff
		std::vector<BoneIds>     m_BoneIds;
		std::vector<BoneWeights> m_BoneWeights;


		// Array object and buffers
		std::vector<Vertex> m_ArrayBuffer;
		std::vector<uint32_t> m_Indices;

		std::vector<float> m_ArrayBufferTest;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;


		bool m_Smooth;
	};
}