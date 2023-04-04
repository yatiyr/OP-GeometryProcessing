#include <Precomp.h>

#include <MainRender/MainRender.h>
#include <Math/Math.h>
#include <GeoProcess/System/CoreSystem/Application.h>
#include <GeoProcess/System/ResourceSystem/ResourceManager.h>

#include <GeoProcess/System/RenderSystem/VertexArray.h>
#include <GeoProcess/System/RenderSystem/Shader.h>
#include <GeoProcess/System/RenderSystem/UniformBuffer.h>
#include <GeoProcess/System/RenderSystem/RenderCommand.h>

#include <GeoProcess/System/RenderSystem/Texture.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <glad/glad.h>


#include <GeoProcess/System/Geometry/Icosphere.h>
#include <GeoProcess/System/Geometry/Cube.h>
#include <GeoProcess/System/Geometry/Plane.h>
#include <GeoProcess/System/Geometry/Quad.h>
#include <GeoProcess/System/Geometry/Model.h>
#include <GeoProcess/System/Geometry/Skybox.h>


#define MAX_DIR_LIGHTS 2
#define MAX_SPOT_LIGHTS 4
#define MAX_CASCADE_SIZE 15
#define MAX_POINT_LIGHTS 10
#define MAX_BONES 100

#include <GeoProcess/System/RenderSystem/EnvironmentMap.h>

namespace GP
{

	struct RenderData
	{
		// ------------ Render Constant ------------ //
		float Epsilon = 0.00000001;

		// ----------- Uniform Buffers ------------- //
		struct CameraData {
			glm::mat4 ViewProjection; glm::mat4 View; glm::mat4 Projection;
			alignas(16) glm::vec3 ViewPos; float Near; float Far;
		} CameraBuffer;

		Ref<UniformBuffer> CameraUniformBuffer;

		struct TransformData {
			glm::mat4 Model;
		} TransformBuffer;

		Ref<UniformBuffer> TransformUniformBuffer;

		// -------- Buffers for Animation -------- //
		struct BoneMatrix { glm::mat4 mat; };
		struct BoneMatricesData { BoneMatrix BoneMatrices[MAX_BONES]; };

		Ref<UniformBuffer> BoneMatricesUniformBuffer;

		// -------- Buffer for Tone Mapping ------- //
		struct ToneMappingSettings { float exposure = 4.0; bool hdr = false; } ToneMappingSettingsBuffer;

		Ref<UniformBuffer> ToneMappingSettingsUniformBuffer;

		// ------- Shaders -------- //
		Ref<Shader> mainShader;
		Ref<Shader> colorShader;
		Ref<Shader> postProcessingShader;
		Ref<Shader> triangleIdShader;
		Ref<Shader> gridShader;
		Ref<Shader> flatColorRenderShader;


		// ------ Meshes ------ //
		Ref<Cube> cube;
		Ref<Plane> plane;
		Ref<Quad> quad;
		Ref<Skybox> skybox;

		// ------- Model ------- //
		Ref<Model> model;
		Ref<EditorMesh> editorMesh;
		glm::mat4 modelTransform = glm::mat4(1.0f);


		// ----- Viewport Settings ----- //
		glm::vec2 ViewportSize{ 0.0f, 0.0f };

		// ----- Framebuffers ----- //
		Ref<Framebuffer> sampleResolveFramebuffer;

		// ---- Renderpasses ---- //
		Ref<RenderPass> finalRenderPass;
		Ref<RenderPass> postProcessingPass;
		Ref<RenderPass> triangleIdFramebufferPass;

		// ----- Environment Map ----- //
		Ref<EnvironmentMap> environmentMap;


		// ----- Renderer Settings ----- //
		bool ShowGrid = true;
	};

	static RenderData s_RenderData;

	void MainRender::Init(float width, float height)
	{
		// Initialize environment map
		s_RenderData.environmentMap = ResourceManager::GetEnvironmentMap("white-gradient-background");
		s_RenderData.environmentMap->GenerateMaps();

		// Initialize meshes
		s_RenderData.skybox = Skybox::Create();
		s_RenderData.plane = Plane::Create();
		s_RenderData.quad = Quad::Create();
		s_RenderData.cube = Cube::Create();

		// Initialize Model
		s_RenderData.model = ResourceManager::GetModel("man0");
		s_RenderData.editorMesh = EditorMesh::Create(s_RenderData.model);

		// Initialize main render settings
		RenderCommand::Enable(MODE::DEPTH_TEST);
		RenderCommand::Enable(MODE::CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0x00);


		// Get Shaders
		s_RenderData.mainShader = ResourceManager::GetShader("Main.glsl");
		s_RenderData.colorShader = ResourceManager::GetShader("ColorShader.glsl");
		s_RenderData.postProcessingShader = ResourceManager::GetShader("PostProcessing.glsl");
		s_RenderData.triangleIdShader = ResourceManager::GetShader("MousePicking.glsl");
		s_RenderData.gridShader = ResourceManager::GetShader("Grid.glsl");
		s_RenderData.flatColorRenderShader = ResourceManager::GetShader("FlatColorRenderShader.glsl");

		// Set Uniform buffer values
		s_RenderData.CameraUniformBuffer = UniformBuffer::Create(sizeof(RenderData::CameraData), 1);
		s_RenderData.TransformUniformBuffer = UniformBuffer::Create(sizeof(RenderData::TransformData), 2);
		s_RenderData.ToneMappingSettingsUniformBuffer = UniformBuffer::Create(sizeof(RenderData::ToneMappingSettings), 3);
		s_RenderData.BoneMatricesUniformBuffer = UniformBuffer::Create(sizeof(RenderData::BoneMatricesData), 4);

		// Set viewport settings
		s_RenderData.ViewportSize.x = width;
		s_RenderData.ViewportSize.y = height;

		// Set Framebuffers

			// Triangle Id Framebuffer
			FramebufferSpecification triangleIdSpec;
			triangleIdSpec.Attachments = { FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
			triangleIdSpec.Width = s_RenderData.ViewportSize.x;
			triangleIdSpec.Height = s_RenderData.ViewportSize.y;

			s_RenderData.triangleIdFramebufferPass = RenderPass::Create(std::string("TriangleId Pass"), triangleIdSpec);

			// Final Framebuffer
			FramebufferSpecification finalFBSpec;
			finalFBSpec.Attachments = { FramebufferTextureFormat::RGBA32F, FramebufferTextureFormat::Depth };
			finalFBSpec.Width = s_RenderData.ViewportSize.x;
			finalFBSpec.Height = s_RenderData.ViewportSize.y;
			finalFBSpec.Samples = 8;

			s_RenderData.finalRenderPass = RenderPass::Create(std::string("Final Pass"), finalFBSpec);

			// Sample Resolve Framebuffer
			FramebufferSpecification sampleResolveFB;
			sampleResolveFB.Attachments = { FramebufferTextureFormat::RGBA32F, FramebufferTextureFormat::Depth };
			sampleResolveFB.Width = s_RenderData.ViewportSize.x;
			sampleResolveFB.Height = s_RenderData.ViewportSize.y;
			s_RenderData.sampleResolveFramebuffer = Framebuffer::Create(sampleResolveFB);

			// Post Processing Final Framebuffer
			FramebufferSpecification postProcessingFBSpec;
			postProcessingFBSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
			postProcessingFBSpec.Width = s_RenderData.ViewportSize.x;
			postProcessingFBSpec.Height = s_RenderData.ViewportSize.y;

			s_RenderData.postProcessingPass = RenderPass::Create(std::string("Post Processing Pass"), postProcessingFBSpec);

	}

	void MainRender::ResizeViewport(float width, float height)
	{
		if (s_RenderData.ViewportSize.x != width || s_RenderData.ViewportSize.y != height)
		{
			// Update viewport size
			s_RenderData.ViewportSize.x = width;
			s_RenderData.ViewportSize.y = height;

			// Resize necessary framebuffers
			s_RenderData.postProcessingPass->GetFramebuffer()->Resize(width, height);
			s_RenderData.sampleResolveFramebuffer->Resize(width, height);
			s_RenderData.finalRenderPass->GetFramebuffer()->Resize(width, height);
			s_RenderData.triangleIdFramebufferPass->GetFramebuffer()->Resize(width, height);
		}
	}

	Ref<Framebuffer> MainRender::GetFinalFramebuffer()
	{
		return s_RenderData.postProcessingPass->GetFramebuffer();
	}

	Ref<Framebuffer> MainRender::GetMainRenderFramebuffer()
	{
		return s_RenderData.finalRenderPass->GetFramebuffer();
	}

	Ref<Framebuffer> MainRender::GetTriangleIDFramebuffer()
	{
		return s_RenderData.triangleIdFramebufferPass->GetFramebuffer();
	}

	float* MainRender::GetExposure()
	{
		return &s_RenderData.ToneMappingSettingsBuffer.exposure;
	}

	void MainRender::SetExposure(float Exposure)
	{
		s_RenderData.ToneMappingSettingsBuffer.exposure = Exposure;
	}

	bool* MainRender::GetHdr()
	{
		return &s_RenderData.ToneMappingSettingsBuffer.hdr;
	}

	void MainRender::SetHdr(bool Hdr)
	{
		s_RenderData.ToneMappingSettingsBuffer.hdr = Hdr;
	}

	void MainRender::SetModelTransform(glm::mat4 t)
	{
		s_RenderData.modelTransform = t;
	}

	glm::mat4& MainRender::GetModelTransform()
	{
		return s_RenderData.modelTransform;
	}

	bool* MainRender::GetShowGrid()
	{
		return &s_RenderData.ShowGrid;
	}

	int* MainRender::GetGeoDistStartIndex()
	{
		return &s_RenderData.editorMesh->m_StartIndex;
	}

	int* MainRender::GetGeoDistEndIndex()
	{
		return &s_RenderData.editorMesh->m_EndIndex;
	}

	int* MainRender::GetGeodesicCalcMethod()
	{
		return &s_RenderData.editorMesh->m_GeodesicDistanceCalcMethod;
	}

	bool* MainRender::GetShowLine()
	{
		return &s_RenderData.editorMesh->m_ShowLine;
	}

	float* MainRender::GetCalcTime()
	{
		return &s_RenderData.editorMesh->m_CalcTime;
	}

	Ref<EditorMesh> MainRender::GetEditorMesh()
	{
		return s_RenderData.editorMesh;
	}

	void MainRender::RenderChain(TimeStep ts)
	{

		s_RenderData.triangleIdFramebufferPass->InvokeCommands(
			[&]()-> void {
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				s_RenderData.triangleIdFramebufferPass->GetFramebuffer()->ClearAttachment(0, -1);
				s_RenderData.triangleIdShader->Bind();

				s_RenderData.TransformBuffer.Model = s_RenderData.modelTransform;
				s_RenderData.TransformUniformBuffer->SetData(&s_RenderData.TransformBuffer, sizeof(RenderData::TransformData));
				s_RenderData.model->Draw();
			}
		);

		s_RenderData.finalRenderPass->InvokeCommands(
			[&]()-> void {
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				glStencilMask(0x00);

				uint32_t ditheringTex = ResourceManager::GetTexture("BayerMatrixDithering")->GetRendererID();
				s_RenderData.TransformBuffer.Model = s_RenderData.modelTransform;
				s_RenderData.TransformUniformBuffer->SetData(&s_RenderData.TransformBuffer, sizeof(RenderData::TransformData));

				s_RenderData.editorMesh->Draw(s_RenderData.mainShader,
											  s_RenderData.colorShader,
											  s_RenderData.flatColorRenderShader,
											  s_RenderData.environmentMap,
											  ditheringTex);

				s_RenderData.environmentMap->RenderSkybox();
			}
		);

		s_RenderData.postProcessingPass->InvokeCommands(
			[&]()-> void {

				s_RenderData.finalRenderPass->GetFramebuffer()->BindRead();
				s_RenderData.sampleResolveFramebuffer->BindDraw();
				glBlitFramebuffer(0, 0, s_RenderData.ViewportSize.x, s_RenderData.ViewportSize.y,
					0, 0, s_RenderData.ViewportSize.x, s_RenderData.ViewportSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

				s_RenderData.postProcessingPass->GetFramebuffer()->Bind();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				s_RenderData.postProcessingShader->Bind();

				uint32_t resolvedImage = s_RenderData.sampleResolveFramebuffer->GetColorAttachmentRendererID(0);
				glBindTextureUnit(0, resolvedImage);

				glm::mat4 model = glm::mat4(1.0f);
				s_RenderData.TransformBuffer.Model = model;
				s_RenderData.TransformUniformBuffer->SetData(&s_RenderData.TransformBuffer, sizeof(RenderData::TransformData));
				s_RenderData.plane->Draw();

			}
		);
	}

	void MainRender::Render(EditorCamera& camera, TimeStep ts)
	{
		// -------------------- Calculate Camera Data -------------------------------- //
		s_RenderData.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_RenderData.CameraBuffer.ViewPos = camera.GetPosition();
		s_RenderData.CameraBuffer.View = camera.GetViewMatrix();
		s_RenderData.CameraBuffer.Projection = camera.GetProjection();
		s_RenderData.CameraBuffer.Near = camera.GetNearClip();
		s_RenderData.CameraBuffer.Far = camera.GetFarClip();
		s_RenderData.CameraUniformBuffer->SetData(&s_RenderData.CameraBuffer, sizeof(RenderData::CameraData));

		s_RenderData.ToneMappingSettingsUniformBuffer->SetData(&s_RenderData.ToneMappingSettingsBuffer, sizeof(RenderData::ToneMappingSettings));

		RenderChain(ts);
	}

	MainRenderStats MainRender::GetStats()
	{
		return MainRenderStats();
	}

}