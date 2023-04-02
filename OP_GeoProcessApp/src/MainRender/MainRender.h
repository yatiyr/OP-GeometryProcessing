#pragma once

#include <GeoProcess/System/RenderSystem/CameraSystem/EditorCamera.h>
#include <GeoProcess/System/RenderSystem/Framebuffer.h>


namespace GP
{

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

		static bool* GetShowGrid();

	private:

	};
}