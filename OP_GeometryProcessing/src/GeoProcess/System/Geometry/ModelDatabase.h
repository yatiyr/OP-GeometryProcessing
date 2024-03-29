#pragma once

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include <glm/glm.hpp>
#include <string>

namespace GP
{
	struct MeshBlueprint
	{
		std::string name;
		std::vector<glm::vec3> vertices;
		std::vector<uint32_t> indices;
	};

	class ModelDatabase
	{
	public:
		ModelDatabase();
		ModelDatabase(std::string name);
		~ModelDatabase();

		void AddModel(aiNode* modelRootNode, const aiScene* modelScene);
		uint32_t GetMeshCount();
		std::string GetName();
		void SetName(std::string name);

		static Ref<ModelDatabase> Create(std::string Name);

		const std::vector<MeshBlueprint>& GetMeshes();

		const std::vector<uint32_t>& GetIndices();

		uint32_t GetVertexCount();
		uint32_t GetIndexCount();
	private:
		std::string m_Name;
		std::vector<MeshBlueprint> m_Meshes;

		uint32_t m_VertexCount;
		uint32_t m_IndexCount;
	};
}
