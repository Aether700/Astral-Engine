#pragma once
#include "Log.h"

#ifndef AE_PLATFORM_WINDOWS
	#error "Astral Engine only supports Windows for now"
#endif

#ifdef AE_DEBUG
	#define AE_ENABLE_ASSERTS
	#define AE_PROFILE
#endif

#ifdef AE_ENABLE_ASSERTS
	#define AE_CORE_ASSERT(exp, ...) { if(!(exp)){ AE_CORE_ERROR(__VA_ARGS__); __debugbreak(); } }
	#define AE_ASSERT(exp, ...) { if (!exp) { AE_ERROR(__VA_ARGS__); __debugbreak(); } }
#else
	#define AE_CORE_ASSERT(exp, format, ...)
	#define AE_ASSERT(exp, ...)
#endif