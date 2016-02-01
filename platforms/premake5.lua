workspace "HolbertonWar"
  configurations {"Debug", "Release"}
  floatingpoint "Fast"
  flags       { "Symbols" }
  location ("../build")

  project "vm"
    debugdir "../cores"
    kind "ConsoleApp"
    language "C"

		filter { "system:linux" }
			links { "X11", "Xxf86vm", "Xrandr", "Xi", "GL", "pthread", "m", "ncurses"}

    files { "../vm/**.h", "../vm/**.c" }
    files { "../common/**.h", "../common/**.c" }
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
