#include <Gp.h>

#include <EntryPoint.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <Editor/EditorLayer.h>

namespace GP
{
    class GpApp : public Application
    {
    public:
        GpApp()
            : Application("Gp App")
        {
            EditorLayer* editorLayer = EditorLayer::CreateEditor();
            PushLayer(editorLayer);
        }

        ~GpApp()
        {

        }
    };

    Application* CreateApplication()
    {
        return new GpApp();
    }
}
