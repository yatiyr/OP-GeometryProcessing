#pragma once

#include <GeoProcess/System/RenderSystem/RendererAPI.h>

namespace GP
{
	class RenderCommand
	{
	public:
		inline static void Init() { s_RendererAPI->Init(); }

		inline static void SetViewport(uint32_t x, 
			                           uint32_t y,
			                           uint32_t width,
			                           uint32_t height) { s_RendererAPI->SetViewport(x, y, width, height); }

		inline static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }

		inline static void Clear() { s_RendererAPI->Clear(); }

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{ 
			s_RendererAPI->DrawIndexed(vertexArray, count);
		}

		inline static void DrawLine(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawLine(vertexArray, count);
		}

		inline static void DrawIndexedBinded(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawIndexedBinded(vertexArray, count);
		}

		inline static void PolygonMode(FACE face, POLYGONMODE mode)
		{
			s_RendererAPI->PolygonMode(face, mode);
		}

		inline static void Enable(MODE mode)
		{
			s_RendererAPI->Enable(mode);
		}

		inline static void Disable(MODE mode)
		{
			s_RendererAPI->Disable(mode);
		}

		inline static void DepthFunc(DEPTHFUNC func)
		{
			s_RendererAPI->DepthFunc(func);
		}


	private:
		static RendererAPI* s_RendererAPI;
	};
}