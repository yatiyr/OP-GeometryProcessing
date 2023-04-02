#pragma once

#include <GeoProcess/System/RenderSystem/CameraSystem/EditorCamera.h>
#include <GeoProcess/System/RenderSystem/Framebuffer.h>

#include <glm/glm.hpp>

namespace GP
{
	struct RenderSpecs
	{
		bool fill = true;
		bool line = false;
		glm::vec4 lineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		bool point;
		glm::vec4 pointColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		
		bool backfaceCulling = true;

		glm::vec3 point1Pos;
		glm::vec3 point2Pos;
	};

	struct MainRenderStats
	{
		uint32_t totalVertices = 0;
	};

	class MainRender
	{
	public:
		static void Init(float width, float height);
		static void Render(EditorCamera& camera, TimeStep ts);
		static void RenderChain(TimeStep ts);

		static MainRenderStats GetStats();
		static void ResizeViewport(float width, float height);

		static Ref<Framebuffer> GetFinalFramebuffer();
		static Ref<Framebuffer> GetMainRenderFramebuffer();
		static Ref<Framebuffer> GetTriangleIDFramebuffer();

		static float* GetExposure();
		static void SetExposure(float Exposure);

		static bool* GetHdr();
		static void SetHdr(bool Hdr);

		static RenderSpecs* GetRenderSpecs();

		static void SetModelTransform(glm::mat4 model);
		static glm::mat4& GetModelTransform();

		static bool* GetShowGrid();

	private:

	};
}