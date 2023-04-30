project "OP_GeometryProcessing"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"


	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "Precomp.h"
	pchsource "src/Config/Precomp.cpp"

	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp",
		"src/**.c",

		"external/stb_image/**.h",
		"external/stb_image/**.cpp",
		"external/tinyexr/**.h",
		"external/tinyexr/**.cpp",
		"external/tinyexr/**.c",
		"external/glm/glm/**.hpp",
		"external/glm/glm/**.inl",
		"external/ImGuizmo/*.h",
		"external/ImGuizmo/*.cpp"
	}

	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}


	includedirs
	{
		"%{IncludeDir.Spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImPlot}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.tinyexr}",
		"%{IncludeDir.Assimp}",
            "%{IncludeDir.Eigen}",
		"%{IncludeDir.Spectra}",
		"src",
		"src/Config"
	}


	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"%{Library.Assimp}"
	}

	filter "files:external/ImGuizmo/**.cpp"
		flags {"NoPCH"}


	filter "configurations:Debug"
		defines "OP_GEOP_DBG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "OP_GEOP_RELEASE"
		runtime "Release"
		optimize "speed"

	filter "configurations:Dist"
		defines "OP_GEOP_DIST"
		runtime "Release"
		optimize "speed"