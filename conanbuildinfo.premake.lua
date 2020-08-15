#!lua
conan_build_type = "Release"
conan_arch = "x86_64"

conan_includedirs = {"C:/Users/Dusan/.conan/data/libsodium/1.0.18/_/_/package/c24028eb2186bbf239014a660463377b0562930e/include"}
conan_libdirs = {"C:/Users/Dusan/.conan/data/libsodium/1.0.18/_/_/package/c24028eb2186bbf239014a660463377b0562930e/lib"}
conan_bindirs = {}
conan_libs = {"sodium"}
conan_system_libs = {}
conan_defines = {}
conan_cxxflags = {}
conan_cflags = {}
conan_sharedlinkflags = {}
conan_exelinkflags = {}

conan_includedirs_libsodium = {"C:/Users/Dusan/.conan/data/libsodium/1.0.18/_/_/package/c24028eb2186bbf239014a660463377b0562930e/include"}
conan_libdirs_libsodium = {"C:/Users/Dusan/.conan/data/libsodium/1.0.18/_/_/package/c24028eb2186bbf239014a660463377b0562930e/lib"}
conan_bindirs_libsodium = {}
conan_libs_libsodium = {"sodium"}
conan_system_libs_libsodium = {}
conan_defines_libsodium = {}
conan_cxxflags_libsodium = {}
conan_cflags_libsodium = {}
conan_sharedlinkflags_libsodium = {}
conan_exelinkflags_libsodium = {}
conan_rootpath_libsodium = "C:/Users/Dusan/.conan/data/libsodium/1.0.18/_/_/package/c24028eb2186bbf239014a660463377b0562930e"

function conan_basic_setup()
    configurations{conan_build_type}
    architecture(conan_arch)
    includedirs{conan_includedirs}
    libdirs{conan_libdirs}
    links{conan_libs}
    links{conan_system_libs}
    defines{conan_defines}
    bindirs{conan_bindirs}
end
