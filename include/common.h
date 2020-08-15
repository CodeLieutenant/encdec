#ifndef ENCDEC_COMMON

#ifdef __cplusplus
extern "C"
{
#endif
#define ENCDEC_COMMON

  // #define ENCDEC_PLATFORM_WINDOWS 0
  // #define ENCDEC_PLATFORM_LINUX 0

#ifdef __WIN32
#undef ENCDEC_PLATFORM_WINDOWS
#define ENCDEC_PLATFORM_WINDOWS 1
#elif __linux__
#undef ENCDEC_PLATFORM_LINUX
#define ENCDEC_PLATFORM_LINUX 1
#endif
#ifdef __cplusplus
}
#endif
#endif
