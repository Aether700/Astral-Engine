workspace "AstralEngine"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	IncludeDir = {}
	IncludeDir["GLFW"] = "AstralEngine/vendor/GLFW/include"
	IncludeDir["Glad"] = "AstralEngine/vendor/Glad/include"
	IncludeDir["stbi"] = "AstralEngine/vendor/stb_image"

include "AstralEngine/vendor/GLFW"
include "AstralEngine/vendor/Glad"

project "AstralEngine"
	location "AstralEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "aepch.h"
	pchsource "AstralEngine/src/aepch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.stbi}"
	}

	links
	{
		"GLFW",
		"Glad"
	}

	filter "system:windows"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"AE_PLATFORM_WINDOWS", 
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "AE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "AE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "AE_DIST"
		runtime "Release"
		optimize "on"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"AstralEngine/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.stbi}"
	}

	links
	{
		"AstralEngine"
	}
	
	filter "system:windows"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"AE_PLATFORM_WINDOWS"
		}


	filter "configurations:Debug"
		defines "AE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "AE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "AE_DIST"
		runtime "Release"
		optimize "on"