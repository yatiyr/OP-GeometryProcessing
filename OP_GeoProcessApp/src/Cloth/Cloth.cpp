#include <Precomp.h>
#include <Cloth/Cloth.h>

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

				ClothParticleProp p;
				p.acceleration = 0.0f;
				p.mass = 0.0f;
				m_ClothParticleProps.push_back(p);
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

			}
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