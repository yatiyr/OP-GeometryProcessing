#include <Precomp.h>
#include <GeoProcess/System/CoreSystem/Layer.h>

namespace GP
{
	Layer::Layer(const std::string& debugName) : m_DebugName(debugName) {}
	Layer::~Layer() {}
}