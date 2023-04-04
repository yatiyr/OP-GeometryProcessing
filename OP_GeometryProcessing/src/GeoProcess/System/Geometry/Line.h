#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <GeoProcess/System/RenderSystem/VertexArray.h>
#include <GeoProcess/System/Geometry/Mesh.h>

namespace GP
{

	struct LineVertex
	{
		glm::vec3 pos;
	};

	// A general purpose 
	class Line : public Mesh
	{
	public:
		Line(std::vector<glm::vec3> vertices);
		static Ref<Line> Create(std::vector<glm::vec3> vertices);

		void SetNewVertices(std::vector<glm::vec3> vertices);
		virtual void Draw() const;
	private:
		std::vector<LineVertex> m_LineArrayBuffer;
		virtual void BuildVertices() override;
	};
}