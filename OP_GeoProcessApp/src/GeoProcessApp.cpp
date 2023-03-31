#include <Gp.h>

#include <EntryPoint.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace GP
{
    class GpApp : public Application
    {
    public:
        GpApp()
            : Application("Gp App")
        {

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
