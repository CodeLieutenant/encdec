
include("conanbuildinfo.premake.lua")

workspace("EncDec")
    conan_basic_setup()
    platforms { "Win64" }
    configurations { "Debug", "Release" }

    filter { "platforms:Win64" }
        system "Windows"
        architecture "x86_64"

    project "Encrypt"
        kind "ConsoleApp"
        language "C"
        targetdir "bin/%{cfg.buildcfg}"

        linkoptions { conan_exelinkflags }

        files { "include/**.h",  "src/encrypt/**.h", "src/encrypt/**.c" }

        filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

        filter "configurations:Release"
        defines { "RELEASE" }
        optimize "On"

    project "Decrypt"
        kind "ConsoleApp"
        language "C"
        targetdir "bin/%{cfg.buildcfg}"

        linkoptions { conan_exelinkflags }

        files { "include/**.h", "src/decrypt/**.h", "src/decrypt/**.c" }

        filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

        filter "configurations:Release"
        defines { "RELEASE" }
        optimize "On"