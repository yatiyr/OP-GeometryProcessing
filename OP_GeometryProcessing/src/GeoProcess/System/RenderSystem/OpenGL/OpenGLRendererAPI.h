#pragma once

#include <GeoProcess/System/RenderSystem/RendererAPI.h>


namespace GP
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawIndexedBinded(const Ref<VertexArray>& vertexArray, uint32_t indexCount) override;
		virtual void DrawLine(const Ref<VertexArray>& vertexArray, uint32_t count) override;
		virtual void Enable(MODE mode) override;
		virtual void Disable(MODE mode) override;
		virtual void DepthFunc(DEPTHFUNC func) override;
		virtual void PolygonMode(FACE face, POLYGONMODE mode) override;
	};
}