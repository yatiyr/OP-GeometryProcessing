#pragma once

#include <GeoProcess/System/CoreSystem/Core.h>
#include <GeoProcess/System/EventSystem/Event.h>
#include <GeoProcess/System/CoreSystem/TimeStep.h>

namespace GP
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(TimeStep time) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}