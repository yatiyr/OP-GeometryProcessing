#pragma once

#include <GeoProcess/System/RenderSystem/CameraSystem/EditorCamera.h>
#include <GeoProcess/System/RenderSystem/Framebuffer.h>
#include <MeshOperations/EditorMesh.h>
#include <MeshOperations/PCADatabase.h>

#include <glm/glm.hpp>

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

		static void SetModelTransform(glm::mat4 model);
		static glm::mat4& GetModelTransform();

		static bool* GetShowGrid();


		static Ref<PCADatabase> GetDatabase();

		static int* GetGeoDistStartIndex();
		static int* GetGeoDistEndIndex();
		static int* GetGeodesicCalcMethod();
		static bool* GetShowLine();
		static float* GetCalcTime();

		static Ref<EditorMesh> GetEditorMesh();

	private:

	};
}