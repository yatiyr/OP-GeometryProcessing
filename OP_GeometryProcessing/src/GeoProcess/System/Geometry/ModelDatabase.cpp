#include <Precomp.h>
#include "ModelDatabase.h"



namespace GP
{
	ModelDatabase::ModelDatabase() {}
	ModelDatabase::ModelDatabase(std::string name) : m_Name(name) {}

	ModelDatabase::~ModelDatabase() {}

	void ModelDatabase::AddModel(aiNode* modelRootNode, const aiScene* scene)
	{
		MeshBlueprint meshBp;
		aiNode* currentNode = modelRootNode;

		if (modelRootNode->mNumMeshes == 0 && modelRootNode->mNumChildren > 0)
			currentNode = modelRootNode->mChildren[0];

		if (currentNode->mNumMeshes > 0)
		{
			aiMesh* mesh = scene->mMeshes[currentNode->mMeshes[0]];

			for (uint32_t i = 0; i < mesh->mNumVertices; i++)
			{
				glm::vec3 vertex = { 0.0f, 0.0f, 0.0f };
				aiVector3D aiVertex = mesh->mVertices[i];

				vertex.x = aiVertex.x;
				vertex.y = aiVertex.y;
				vertex.z = aiVertex.z;

				meshBp.vertices.push_back(vertex);
			}

			for (uint32_t i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];

				for (uint32_t j = 0; j < face.mNumIndices; j++)
				{
					meshBp.indices.push_back(face.mIndices[j]);
				}
			}

			meshBp.name = mesh->mName.C_Str();

			m_Meshes.push_back(meshBp);
		}
			
	}

	std::string ModelDatabase::GetName()
	{
		return m_Name;
	}

	void ModelDatabase::SetName(std::string name)
	{
		m_Name = name;
	}

	uint32_t ModelDatabase::GetMeshCount()
	{
		return m_Meshes.size();
	}

	Ref<ModelDatabase> ModelDatabase::Create(std::string name)
	{
		return std::make_shared<ModelDatabase>(name);
	}
}

