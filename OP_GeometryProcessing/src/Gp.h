#pragma once

#define GLM_FORCE_AVX
#define GL_FORCE_SSE2


#include <GeoProcess/System/CoreSystem/Application.h>
#include <GeoProcess/System/CoreSystem/Layer.h>
#include <GeoProcess/System/CoreSystem/Logger.h>

#include <GeoProcess/System/CoreSystem/TimeStep.h>
#include <GeoProcess/System/InputSystem/Input.h>
#include <GeoProcess/System/InputSystem/KeyCodes.h>
#include <GeoProcess/System/InputSystem/MouseButtonCodes.h>

#include <GeoProcess/System/GuiSystem/ImGuiLayer.h>

#include <GeoProcess/System/RenderSystem/Renderer.h>
#include <GeoProcess/System/RenderSystem/RenderCommand.h>

#include <GeoProcess/System/RenderSystem/Buffer.h>
#include <GeoProcess/System/RenderSystem/Shader.h>
#include <GeoProcess/System/RenderSystem/Framebuffer.h>
#include <GeoProcess/System/RenderSystem/Texture.h>
#include <GeoProcess/System/RenderSystem/VertexArray.h>

#include <GeoProcess/System/RenderSystem/CameraSystem/Camera.h>
#include <GeoProcess/System/RenderSystem/CameraSystem/EditorCamera.h>
