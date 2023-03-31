include "dependencies.lua"

workspace "GeoProcess"
	architecture "x86_64"
	startproject "OP_GeoProcessApp"


	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Lib"
	include "OP_GeometryProcessing"
group ""

group "App"
	include "OP_GeoProcessApp"
group ""

group "ThirdParty"
	include "OP_GeometryProcessing/external/glfw"
	include "OP_GeometryProcessing/external/glad"
	include "OP_GeometryProcessing/external/imgui"
group ""