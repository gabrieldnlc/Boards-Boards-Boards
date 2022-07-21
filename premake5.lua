workspace "SimpleBoard"
	startproject "SimpleBoard"
	architecture "x86"
	configurations{
		"Debug",
		"Release"
	}
	platforms{ 
		"Win32"
	}

output_dir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
target_dir = ("bin/" .. output_dir .. "/%{prj.name}")

project "SimpleBoard"
	location "SimpleBoard"
	kind "ConsoleApp"
	language "C++"
	defines{"IMGUI_DEFINE_MATH_OPERATORS"}

	targetdir (target_dir)
	objdir ("bin-int/" .. output_dir .. "/%{prj.name}")
	debugdir ("bin/" .. output_dir .. "/%{prj.name}")

	postbuildcommands {
		("{COPY} extern/SDL2/lib/x86/SDL2.dll ../" .. target_dir .. "/"),
		("{COPYDIR} scripts ../" .. target_dir .. "/scripts")
	}

	files{
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/extern/imgui/imgui.cpp",
		"%{prj.name}/extern/imgui/imgui_draw.cpp",
		"%{prj.name}/extern/imgui/imgui_widgets.cpp",
		"%{prj.name}/extern/imgui/imgui_tables.cpp",
		"%{prj.name}/extern/imgui/backends/imgui_impl_dx11.cpp",
		"%{prj.name}/extern/imgui/backends/imgui_impl_sdl.cpp",
		"%{prj.name}/extern/imgui/imgui_demo.cpp",
		"%{prj.name}/extern/imgui/misc/cpp/imgui_stdlib.cpp",
	}

	includedirs{
		"%{prj.name}/src",
		"%{prj.name}/extern/Lua/include",
		"%{prj.name}/extern/sol",
		"%{prj.name}/extern/SDL2/include",
		"%{prj.name}/extern/nativefiledialog/include",
		"%{prj.name}/extern/imgui/",
		"%{prj.name}/extern/imgui/backends",
		"%{prj.name}/extern/imgui/misc/cpp",
	}
	libdirs{
		"%{prj.name}/extern/Lua/lib",
		"%{prj.name}/extern/SDL2/lib/x86",
	}
	
	links{
		"lua54.lib",
		"SDL2.lib",
		"SDL2main.lib",
		"d3d11.lib",
		"d3dcompiler.lib",
		"dxgi.lib",
	}

	filter "system:windows"
		cppdialect "C++latest"
		systemversion "latest"

		defines{
			"WIN32",
		}

		filter "platforms:x86"
        	system "Windows"
        	architecture "x86"

		filter "configurations:Debug"
			defines "BOARD_DEBUG"
			symbols "On"
			runtime "Debug"
			libdirs{
				"%{prj.name}/extern/nativefiledialog/lib/Debug/x86",
			}
			links{
				"nfd_d.lib"
			}

		filter "configurations:Release"
			defines "BOARD_RELEASE"
			optimize "On"
			runtime "Release"
			libdirs{
				"%{prj.name}/extern/nativefiledialog/lib/Release/x86",
			}
			links{
				"nfd.lib"
			}

project "Tests"
	location "Tests"
	kind "ConsoleApp"
	language "C++"

	targetdir (target_dir)
	objdir ("bin-int/" .. output_dir .. "/%{prj.name}")
	debugdir ("bin/" .. output_dir .. "/%{prj.name}")

	postbuildcommands {
		--("{COPYDIR} scripts ../" .. target_dir .. "/scripts"),
		("{COPYDIR} ../SimpleBoard/scripts/serpent ../" .. target_dir .. "/scripts/serpent"),
	}

	links { "SimpleBoard", "PostContainer.obj", "LuaStack.obj", "ParsingStrategies.obj", "lua54.lib" }
	files{
		"%{prj.name}/**.cpp",

	}
	includedirs{
		"%{prj.name}/Catch2/",
		"SimpleBoard/src/",
		"SimpleBoard/extern/Lua/include",
		"SimpleBoard/extern/sol",
		"SimpleBoard/extern/imgui/",
	}
	libdirs{
		"bin-int/" .. output_dir .. "/SimpleBoard",
		"SimpleBoard/extern/Lua/lib",
	}

	filter "system:windows"
		cppdialect "C++latest"
		systemversion "latest"

		defines{
			"WIN32",
		}

		filter "platforms:x86"
        	system "Windows"
        	architecture "x86"

		filter "configurations:Debug"
			defines "BOARD_DEBUG"
			symbols "On"
			runtime "Debug"

		filter "configurations:Release"
			defines "BOARD_RELEASE"
			optimize "On"
			runtime "Release"