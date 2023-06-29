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


#define DAMPING 0.01
#define TIMESTEP 0.5 * 0.5 * 0.025
#define CONSTRAINT_ITERATIONS 3

namespace GP
{
	struct ClothParticle
	{
		bool moving;
		uint32_t id;
		float mass;
		glm::vec3 acceleration;
		glm::vec3 pos;
		glm::vec3 oldPos;
		glm::vec3 normal;


		void addForce(glm::vec3 f)
		{
			acceleration += f / mass;
		}

		void step()
		{
			if (moving)
			{
				glm::vec3 temp = pos;
				pos = pos + (pos - oldPos) * (float)(1.0 - DAMPING) + acceleration * (float)(TIMESTEP);
				oldPos = temp;
				acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
			}
		}

		void resetAcceleration() { acceleration = glm::vec3(0.0f, 0.0f, 0.0f); }
		void offsetPosition(glm::vec3 offset) { if (moving) pos += offset; }
		void disableMoving() { moving = false; }

		void addNormal(glm::vec3 n)
		{
			normal += glm::normalize(n);
		}

		void resetNormal() { normal = glm::vec3(0.0f, 0.0f, 0.0f); }

	};

	struct ClothConstraint
	{
		float restDistance;
		ClothParticle* particle1;
		ClothParticle* particle2;

		ClothConstraint(ClothParticle* p1, ClothParticle* p2)
		{
			particle1 = p1;
			particle2 = p2;
			restDistance = glm::length(p1->pos - p2->pos);
		}

		void apply()
		{
			glm::vec3 p1p2 = particle2->pos - particle1->pos;
			float dist = glm::length(p1p2);
			glm::vec3 correction = p1p2 * (1 - restDistance / dist);
			glm::vec3 correctionHalf = correction / 2.0f;
			particle1->offsetPosition(correctionHalf);
			particle2->offsetPosition(-correctionHalf);
		}
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

		void ApplyForceToTriangle(ClothParticle* p1,
			ClothParticle* p2,
			ClothParticle* p3,
			const glm::vec3& direction);

		void ApplyGravity();

		void ApplyWind(const glm::vec3& direction);

		void Step();


		void SphereCollision(glm::mat4 sphereTransform, float radius);

		void UpdateNormals();
		
		void UpdateVertexBuffer();

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
		std::vector<ClothParticle> m_ClothParticles;
		std::vector<ClothConstraint> m_Constraints;

		std::vector<uint32_t> m_Indices;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};

}