#include <Precomp.h>
#include <GeoProcess/System/Geometry/Line.h>
#include <GeoProcess/System/RenderSystem/RenderCommand.h>

namespace GP
{
	Line::Line(std::vector<glm::vec3> vertices)
	{
		m_Vertices = vertices;
		BuildVertices();
	}

	Ref<Line> Line::Create(std::vector<glm::vec3> vertices)
	{
		return std::make_shared<Line>(vertices);
	}

	void Line::SetNewVertices(std::vector<glm::vec3> vertices)
	{
		ClearArrays();
		m_Vertices.clear();
		m_Vertices = vertices;

		BuildVertices();
	}

	void Line::BuildVertices()
	{

		m_LineArrayBuffer.clear();

		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			LineVertex vert;
			vert.pos = m_Vertices[i];
			m_LineArrayBuffer.push_back(vert);
		}

		//if (m_VertexArray.get())
			//delete m_VertexArray.get();

		m_VertexArray = VertexArray::Create();
		m_VertexBuffer = VertexBuffer::Create(&m_LineArrayBuffer[0], m_LineArrayBuffer.size() * sizeof(LineVertex));
		m_VertexBuffer->SetLayout(
			{
				{ ShaderDataType::Float3, "a_Position"    },
			}
		);

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
	}

	void Line::Draw() const
	{
		RenderCommand::DrawLine(m_VertexArray, m_Vertices.size());
	}
}
