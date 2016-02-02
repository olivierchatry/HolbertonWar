dofile "premake-utils.lua"

os.mkdir("../build")
os.copydir("../vm/shaders", "../build/shaders")
os.copydir("../vm/data", "../build/data")
filter "system:windows"
	os.copyfile("../externals/glfw/3.1.2/lib/windows/x86_64/glfw3.dll", "../build/glfw3.dll")

workspace "HolbertonWar"
	location 		("../build")
	targetdir		("../build")
  configurations {"Debug", "Release"}
	defines { "GLEW_STATIC", "GLM_FORCE_RADIANS" }

	filter "action:vs*"
		defines     { "_CRT_SECURE_NO_WARNINGS" }

  project "vm"
    debugdir "../cores"
    kind "ConsoleApp"
    language "C"

		files {
			"../vm/**.c",
			"../vm/**.h",
			"../common/**.c",
			"../common/**.h",
			"../vm/shaders/*.frag",
			"../vm/shaders/*.vert",
			"../externals/glew/src/*.c",
			"../externals/stb/*.h"
		}

		includedirs {
			"../externals/glew/include",
			"../externals/glm",
			"../externals/stb"
		}

		filter "system:linux"
			toolset "clang"
			includedirs "../externals/glfw/include"
			if os.is64bit() then
				libdirs "../externals/glfw/lib/linux/x86_64"
			else
				libdirs "../externals/glfw/lib/linux/x86"
			end
			links {"glfw3", "X11", "Xxf86vm", "Xrandr", "Xi", "GL", "pthread", "m", "ncurses"}

		filter "system:macosx"
			platforms 	{"Universal"}
			includedirs "../externals/glfw/include"
			libdirs 	"../externals/glfw/lib/osx"
			links       { "glfw3", "OpenGL.framework", "CoreFoundation.framework", "CoreVideo.framework", "Cocoa.framework", "IOKit.framework", "AGL.framework"}
			buildoptions { "-mmacosx-version-min=10.4" }
			linkoptions  { "-mmacosx-version-min=10.4" }


		filter "system:windows"
			os.copyfile("../externals/glfw/3.1.2/lib/windows/x86_64/glfw3.dll", "../build/glfw3.dll")
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
    debugdir "../cores"
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
