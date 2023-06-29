#include <Precomp.h>
#include <Cloth/Cloth.h>

#include <Math/Math.h>

#include <GeoProcess/System/RenderSystem/RenderCommand.h>
#include <glad/glad.h>
#include <math.h>

namespace GP
{
	// Size will be divided into divisor amount of
	// sectors
	Cloth::Cloth(uint32_t size, uint32_t divisor)
	{
		InitializeArrayBuffer(size, divisor);
		SetupMesh();
	}

	Ref<Cloth> Cloth::Create(uint32_t size, uint32_t divisor)
	{
		return std::make_shared<Cloth>(size, divisor);
	}

	void Cloth::InitializeArrayBuffer(uint32_t size, uint32_t divisor)
	{
		m_ArrayBuffer.clear();

		double step = (double)size / (double)divisor;

		double halfSize = (double)size / 2.0;

		int i = 0;
		int j = 0;

		int id = 0;
		// Cloth will be initialized at xy plane so
		// z coords will always be 0
		for (double y = -halfSize, i=0; i <= divisor; y += step, i++)
		{
			for (double x = -halfSize, j=0; j <= divisor; x += step, j++)
			{
				ClothVertex v;
				v.Pos = glm::vec3(x, y, 0.0f);
				v.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
				v.TexCoord = glm::vec2((x + halfSize) / size, (y + halfSize) / size);
				m_ArrayBuffer.push_back(v);

				ClothParticle p;
				p.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
				p.mass = 1.0f;
				p.id = id;
				p.moving = (i==divisor) ? false : true;
				p.pos = glm::vec3(x, y, 0.0f);
				p.oldPos = glm::vec3(x, y, 0.0f);
				p.normal = glm::vec3(0.0f, 0.0f, 1.0f);

				m_ClothParticles.push_back(p);
				id++;
			}
		}

		// The size of particles is (divisor + 1) * (divisor + 1)
		for (int i = 0; i < divisor; i++)
		{
			for (int j = 0; j < divisor; j++)
			{
				// 1st triangle
				//        3
				//      / |
				//     /  |
				//   1 -- 2
				m_Indices.push_back(i * (divisor + 1) + j);
				m_Indices.push_back(i * (divisor + 1) + j + 1);
				m_Indices.push_back((i + 1) * (divisor + 1) + j + 1);

				// 2nd triangle
				//  3 -- 2
				//  |  / 
				//  | /  
				//  1 
				m_Indices.push_back(i * (divisor + 1) + j);
				m_Indices.push_back((i + 1) * (divisor + 1) + j + 1);
				m_Indices.push_back((i + 1) * (divisor + 1) + j);

				// Constraints
				//  2 --- 3
				//  |     |
				//  |     |
				//  1 --- 4
				int index1 = i * (divisor + 1) + j;
				int index2 = (i + 1) * (divisor + 1) + j;
				int index3 = (i + 1) * (divisor + 1) + j + 1;
				int index4 = i * (divisor + 1) + j + 1;

				ClothParticle* p1 = &m_ClothParticles[index1];
				ClothParticle* p2 = &m_ClothParticles[index2];
				ClothParticle* p3 = &m_ClothParticles[index3];
				ClothParticle* p4 = &m_ClothParticles[index4];

				m_Constraints.push_back(ClothConstraint(p1, p2));
				m_Constraints.push_back(ClothConstraint(p1, p4));
				m_Constraints.push_back(ClothConstraint(p2, p3));
				m_Constraints.push_back(ClothConstraint(p3, p4));

			}
		}
	}

	void Cloth::ApplyForceToTriangle(ClothParticle* p1,
		ClothParticle* p2,
		ClothParticle* p3,
		const glm::vec3& direction)
	{
		glm::vec3 normal = ComputeFaceNormal(p1->pos, p2->pos, p3->pos);
		glm::vec3 normalDir = glm::normalize(normal);
		glm::vec3 force = normal * (glm::dot(normalDir, direction));
		p1->addForce(force);
		p2->addForce(force);
		p3->addForce(force);
	}

	void Cloth::Step()
	{
		ApplyGravity();
		ApplyWind(glm::vec3(0.0f, 0.0f, -4.0f * TIMESTEP));

		for (int i = 0; i < CONSTRAINT_ITERATIONS; i++)
		{
			for (auto& constraint : m_Constraints)
			{
				constraint.apply();
			}
		}

		for (auto& particle : m_ClothParticles)
		{
			particle.step();
		}

		UpdateNormals();
		UpdateVertexBuffer();
	}


	void Cloth::SphereCollision(glm::mat4 sphereTransform, float radius)
	{
		glm::vec3 translation;
		glm::vec3 rotation;
		glm::vec3 scale;

		Math::DecomposeTransform(sphereTransform, translation, rotation, scale);

		// Scale will always be uniform
		float r = radius * scale.x;

		glm::vec3 center = translation;

		for (auto& particle : m_ClothParticles)
		{
			glm::vec3 v = particle.pos - center;
			float dist = glm::length(v);

			if (dist < r)
			{
				particle.offsetPosition(glm::normalize(v) * (r - dist) * 1.4f);
			}
		}
	}


	void Cloth::UpdateNormals()
	{
		for (auto& particle : m_ClothParticles)
		{
			particle.resetNormal();
		}

		for (int i = 0; i < m_Indices.size(); i += 3)
		{
			ClothParticle* p1 = &m_ClothParticles[m_Indices[i]];
			ClothParticle* p2 = &m_ClothParticles[m_Indices[i + 1]];
			ClothParticle* p3 = &m_ClothParticles[m_Indices[i + 2]];

			glm::vec3 norm = ComputeFaceNormal(p1->pos, p2->pos, p3->pos);

			p1->addNormal(norm);
			p2->addNormal(norm);
			p3->addNormal(norm);
		}

		for (auto& particle : m_ClothParticles)
		{
			particle.normal = glm::normalize(particle.normal);
		}
	}

	void Cloth::UpdateVertexBuffer()
	{
		for (int i = 0; i < m_ArrayBuffer.size(); i++)
		{
			m_ArrayBuffer[i].Normal = m_ClothParticles[i].normal;
			m_ArrayBuffer[i].Pos = m_ClothParticles[i].pos;
		}

		m_VertexArray->Bind();
		m_VertexBuffer->SetData(&m_ArrayBuffer[0], m_ArrayBuffer.size() * sizeof(ClothVertex));
	}

	void Cloth::ApplyGravity()
	{
		for (auto& particle : m_ClothParticles)
		{
			particle.addForce(glm::vec3(0.0f, -9.8f * particle.mass * TIMESTEP, 0.0f));
		}
	}

	void Cloth::ApplyWind(const glm::vec3& direction)
	{
		for (int i = 0; i < m_Indices.size(); i += 3)
		{
			ClothParticle* p1 = &m_ClothParticles[m_Indices[i]];
			ClothParticle* p2 = &m_ClothParticles[m_Indices[i + 1]];
			ClothParticle* p3 = &m_ClothParticles[m_Indices[i + 2]];

			ApplyForceToTriangle(p1, p2, p3, direction);
		}
	}
	void Cloth::SetupMesh()
	{
		m_VertexArray = VertexArray::Create();

		m_VertexBuffer = VertexBuffer::CreateDynamic(m_ArrayBuffer.size() * sizeof(ClothVertex));
		m_VertexBuffer->SetData(&m_ArrayBuffer[0], m_ArrayBuffer.size() * sizeof(ClothVertex));
		m_VertexBuffer->SetLayout(
			{
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float3, "a_Normal"   },
				{ ShaderDataType::Float2, "a_TexCoord" }
			}
		);

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_IndexBuffer = IndexBuffer::Create(&m_Indices[0], m_Indices.size());
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void Cloth::AddIndices(uint32_t i1, uint32_t i2, uint32_t i3)
	{
	}


	void Cloth::Draw(Ref<Shader> mainShader,
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

		glLineWidth(1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// RenderCommand::DrawIndexedBinded(m_VertexArray, m_Indices.size());
	}

	glm::vec3 Cloth::ComputeFaceNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
	{
		const float EPSILON = 0.000001f;

		glm::vec3 normal(0.0f);

		glm::vec3 e1 = v2 - v1;
		glm::vec3 e2 = v3 - v1;

		normal = glm::cross(e1, e2);
		if (glm::length(normal) > EPSILON)
		{
			normal = glm::normalize(normal);
			return normal;
		}

		return glm::vec3(0.0f);
	}

}