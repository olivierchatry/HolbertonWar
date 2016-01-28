workspace "HolbertonWar"
  configurations {"Debug", "Release"}
  project "vm"
    kind "ConsoleApp"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"
    files { "../vm/**.h", "../vm/**.c" }
    files { "../common/**.h", "../common/**.c" }
    filter "configurations:Debug"
       defines { "DEBUG" }
       flags { "Symbols" }
    filter "configurations:Release"
       defines { "NDEBUG" }
       optimize "On"
   project "asm"
     kind "ConsoleApp"
     language "C"
     targetdir "bin/%{cfg.buildcfg}"
     files { "../asm/**.h", "../asm/**.c" }
     files { "../common/**.h", "../common/**.c" }
     filter "configurations:Debug"
        defines { "DEBUG" }
        flags { "Symbols" }
     filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
