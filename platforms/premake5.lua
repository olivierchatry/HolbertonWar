dofile "premake-utils.lua"

os.mkdir("../build")
os.mkdir("../build/bin")
os.copydir("../vm/display/gl/data", "../build/bin/data")

local action = _ACTION or ""

workspace "HolbertonWar"
	location 		("../build/" .. action)
	targetdir		("../build/bin")

  configurations {"Debug", "Release"}
	defines { "GLEW_STATIC", "GLM_FORCE_RADIANS" }

	filter "action:vs*"
		defines     { "_CRT_SECURE_NO_WARNINGS" }

  project "vm"
    kind "ConsoleApp"
    language "C"

		files {
			"../vm/**.c",
			"../vm/**.h",
			"../common/**.c",
			"../common/**.h",
			"../vm/display/data/shaders/*.frag",
			"../vm/display/data/shaders/*.vert",
			"../externals/glew/src/*.c",
			"../externals/stb/*.h",
			"../externals/imgui/examples/libs/gl3w/GL/*.c",
			"../externals/imgui/examples/libs/gl3w/GL/*.h"
		}
		language "C++"
		files {
			"../vm/debugger/**.cpp",
			"../vm/debugger/**.h",
			"../externals/imgui/imgui.cpp",
			"../externals/imgui/imgui_draw.cpp",
			"../externals/imgui/examples/opengl3_example/imgui_impl_glfw_gl3.cpp",
			"../externals/imgui/examples/opengl3_example/imgui_impl_glfw_gl3.h",
			"../externals/imgui/*.h"
		}

		includedirs {
			"../externals/glew/include",
			"../externals/glm",
			"../externals/stb",
			"../externals/imgui",
			"../externals/imgui/examples/opengl3_example",
			"../externals/imgui/examples/libs/gl3w"
		}

		filter "system:linux"
			toolset "clang"
			includedirs "../externals/glfw/include"
			if os.is64bit() then
				libdirs "../externals/glfw/lib/linux/x86_64"
			else
				libdirs "../externals/glfw/lib/linux/x86"
			end
			links {"glfw3", "X11", "Xxf86vm", "Xrandr", "Xi", "GL", "pthread", "m", "dl"}

		filter "system:macosx"
			platforms 	{"Universal"}
			defines {"MACOSX"}
			includedirs "../externals/glfw/include"
			libdirs 	"../externals/glfw/lib/osx"
			links       { "glfw3", "OpenGL.framework", "CoreFoundation.framework", "CoreVideo.framework", "Cocoa.framework", "IOKit.framework", "AGL.framework"}
			buildoptions { "-mmacosx-version-min=10.4" }
			linkoptions  { "-mmacosx-version-min=10.4" }


		filter "system:windows"
			os.copyfile("../externals/glfw/3.1.2/lib/windows/x86_64/glfw3.dll", "../build/bin/glfw3.dll")
			architecture ("x86_64")
			libdirs "../externals/glfw/3.1.2/lib/windows/x86_64"
			includedirs "../externals/glfw/3.1.2/include"
			links {"opengl32", "glfw3dll", "Ws2_32"}

		filter "configurations:Debug"
			defines { "DEBUG" }
			flags { "Symbols" }
		filter "configurations:Release"
			defines { "NDEBUG" }
			optimize "On"


	project "asm"
    kind "ConsoleApp"
    language "C"
    files { "../asm/**.h", "../asm/**.c" }
    files { "../common/**.h", "../common/**.c" }
    filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }
    filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
