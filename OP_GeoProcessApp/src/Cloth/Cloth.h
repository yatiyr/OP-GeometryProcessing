#pragma once

#include <GeoProcess/System/RenderSystem/VertexArray.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GeoProcess/System/RenderSystem/Shader.h>
#include <GeoProcess/System/RenderSystem/EnvironmentMap.h>

#include <vector>
#include <unordered_map>

#include <MeshOperations/EditorMesh.h>

namespace GP
{
	struct ClothParticleProp
	{
		float mass;
		float acceleration;
	};

	struct ClothVertex
	{
		glm::vec3 Pos;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};



	// Original Cloth will always
	// be square shaped
	class Cloth
	{
	public:
		Cloth(uint32_t size, uint32_t divisor);

		void BuildVertices() {}

		static Ref<Cloth> Create(uint32_t size, uint32_t divisor);

		void InitializeArrayBuffer(uint32_t size, uint32_t divisor);
		void SetupMesh();

		void AddIndices(uint32_t i1, uint32_t i2, uint32_t i3);

		void ClearArrays();

		uint32_t GetVertexCount() const;
		uint32_t GetNormalCount() const;
		uint32_t GetTriangleCount() const;

		void Draw(Ref<Shader> mainShader,
			Ref<Shader> colorShader,
			Ref<Shader> singleColorShader,
			Ref<EnvironmentMap> envMap,
			uint32_t ditheringTex) const;
	public:
		RenderSpecs m_RenderSpecs;
	protected:
		glm::vec3 ComputeFaceNormal(const glm::vec3& v1,
			const glm::vec3& v2,
			const glm::vec3& v3);

		std::vector<ClothVertex> m_ArrayBuffer;
		std::vector<ClothParticleProp> m_ClothParticleProps;

		std::vector<uint32_t> m_Indices;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};

}