-- Include Directories


IncludeDir = {}

IncludeDir["GLFW"]        = "%{wks.location}/OP_GeometryProcessing/external/GLFW/include"
IncludeDir["Spdlog"]      = "%{wks.location}/OP_GeometryProcessing/external/spdlog/include"
IncludeDir["Glad"]        = "%{wks.location}/OP_GeometryProcessing/external/Glad/include"
IncludeDir["ImGui"]       = "%{wks.location}/OP_GeometryProcessing/external/imgui"
IncludeDir["ImPlot"]      = "%{wks.location}/OP_GeometryProcessing/external/implot"
IncludeDir["glm"]         = "%{wks.location}/OP_GeometryProcessing/external/glm"
IncludeDir["stb_image"]   = "%{wks.location}/OP_GeometryProcessing/external/stb_image"
IncludeDir["tinyexr"]     = "%{wks.location}/OP_GeometryProcessing/external/tinyexr"

IncludeDir["ImGuizmo"]    = "%{wks.location}/OP_GeometryProcessing/external/ImGuizmo"
IncludeDir["Assimp"]      = "%{wks.location}/OP_GeometryProcessing/external/assimp/include"


LibraryDir = {}
LibraryDir["Assimp"] = "%{wks.location}/OP_GeometryProcessing/external/assimp/lib/Release"

Library = {}
Library["Assimp"] = "%{LibraryDir.Assimp}/assimp-vc143-mt.lib"
