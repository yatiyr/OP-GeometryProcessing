#pragma once

#include <GeoProcess/System/RenderSystem/RenderCommand.h>

namespace GP
{
	class Renderer
	{
	public:

		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void BeginScene();
		static void EndScene();

		static void Submit();

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};
}