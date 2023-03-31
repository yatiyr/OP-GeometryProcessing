#pragma once

#include <GeoProcess/System/RenderSystem/VertexArray.h>


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <GeoProcess/System/Geometry/Mesh.h>

namespace GP
{
	class Skybox : public Mesh
	{
	public:
		Skybox();
		static Ref<Skybox> Create();
	private:
		virtual void BuildVertices() override;
	};
}