#pragma once

#include <GeoProcess/System/Geometry/Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <unordered_map>

#include <string>

#include <GeoProcess/System/Animation/Animation.h>
#include <GeoProcess/System/Animation/AnimationHandler.h>

namespace GP
{
	// 




	// These mesh structs will be turned into entities
	// in the editor and games
	struct ModelMesh
	{
		std::string Name;
		Ref<Mesh> Mesh;
		aiMaterial* Material;
	};

	struct ModelNode
	{
		std::string Name;
		ModelNode* Parent;
		glm::mat4 LocalTransformation;
		std::vector<ModelMesh> Meshes;
		std::vector<ModelNode*> Children;
	};

	class Model
	{
	public:
		Model(aiNode* rootNode, const aiScene* scene);
		void ProcessNode(aiNode* node, const aiScene* scene, ModelNode* currentNode, ModelNode* parentNode, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCounter);
		ModelMesh ProcessMesh(aiMesh* mesh, const aiScene* scene, aiNode* currentNode, std::unordered_map<std::string, BoneInfo>& boneInfoMap, int& BoneCounter);

		ModelMesh GetMesh(uint32_t index);

		static Ref<Model> Create(aiNode* rootNode, const aiScene* scene);

		void UpdateAnimation(float deltaTime);
		void ChangeAnimation(int animIndex);
		void Draw();

		BoneMatricesData& GetFinalBoneMatrices();

		std::string GetName() const { return m_Name; }
		void SetName(std::string name) { m_Name = name; }

	private:
		ModelNode* m_RootNode;
		std::string m_Name;
		std::vector<ModelMesh> m_ModelMeshes;
		std::vector<glm::vec3> m_ShortestPathVertices;

		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
		int m_BoneCounter = 0;

		AnimationHandler m_AnimationHandler;
		std::vector<Animation> m_Animations;
	};
}