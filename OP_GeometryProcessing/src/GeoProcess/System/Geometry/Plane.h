#pragma once

#include <GeoProcess/System/RenderSystem/VertexArray.h>


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <GeoProcess/System/Geometry/Mesh.h>

namespace GP
{
	// A general purpose 
	class Plane : public Mesh
	{
	public:
		Plane();
		static Ref<Plane> Create();

	private:
		virtual void BuildVertices() override;
	};
}