
include("conanbuildinfo.premake.lua")

workspace "EncDec"
    conan_basic_setup()
    platforms { "Linux", "Win64" }
    configurations { "Debug", "Release" }
    includedirs{"include"}

    filter { "platforms:Win64" }
        system "Windows"
        architecture "x86_64"

    project "encdec"
        kind "ConsoleApp"
        language "C"
        targetdir "bin/%{cfg.buildcfg}"

        linkoptions { conan_exelinkflags }

        files { "include/**.h", "libs/**.c",  "src/encrypt/**.h", "src/encrypt/**.c" }

        filter "configurations:Debug"
            defines { "DEBUG", "LOG_USE_COLOR" }
            optimize "Off"
            symbols "On"

        filter "configurations:Release"
            defines { "RELEASE", "LOG_USE_COLOR" }
            optimize "On"
        filter { "platforms:Linux"}
            links {"m"}
