#include <Precomp.h>
#include <GeoProcess/System/RenderSystem/Renderer.h>


namespace GP
{
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::Init() { RenderCommand::Init(); }
	void Renderer::OnWindowResize(uint32_t width, uint32_t height) { RenderCommand::SetViewport(0, 0, width, height); }
	void Renderer::BeginScene() {}
	void Renderer::EndScene() {}

	void Renderer::Submit() {}
}