project "OP_GeoProcessApp"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")


	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp",
		"src/**.c"
	}

	includedirs
	{
		"%{wks.location}/OP_GeometryProcessing/external/spdlog/include",
		"%{wks.location}/OP_GeometryProcessing/src",
		"%{wks.location}/OP_GeometryProcessing/src/Config",
		"%{wks.location}/OP_GeometryProcessing/external",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Assimp}",
		"src"
	}

	links
	{
		"OP_GeometryProcessing"
	}

	postbuildcommands
	{
		'{COPY} "%{wks.location}/OP_GeometryProcessing/assets" "%{cfg.targetdir}/assets"',
		'{COPY} "%{wks.location}/OP_GeometryProcessing/external/assimp/bin/Release/assimp-vc143-mt.dll" "%{cfg.targetDir}"'
	}

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
