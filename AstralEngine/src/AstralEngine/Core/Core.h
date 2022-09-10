#pragma once
#include "Log.h"

#ifndef AE_PLATFORM_WINDOWS
	#error "Astral Engine only supports Windows for now"
#endif

#ifdef AE_DEBUG
	#define AE_PROFILE
	
	// assert macros
	#define AE_ENABLE_ASSERTS // global assert macro. Asserts will only work if this macro is defined
	//#define AE_ECS_ASSERTS
	//#define AE_DATASTRUCT_ASSERTS
	#define AE_RENDER_ASSERTS
#endif

#ifdef AE_ENABLE_ASSERTS
	#define AE_CORE_ASSERT(exp, ...) { if(!(exp)){ AE_CORE_ERROR(__VA_ARGS__); __debugbreak(); } }
	#define AE_ASSERT(exp, ...) { if (!exp) { AE_ERROR(__VA_ARGS__); __debugbreak(); } }
#else
	#define AE_CORE_ASSERT(exp, format, ...)
	#define AE_ASSERT(exp, ...)
#endif

// specific core asserts so they can be partially turned on and off
#ifdef AE_ECS_ASSERTS
	#define AE_ECS_ASSERT(exp, ...) AE_CORE_ASSERT(exp, __VA_ARGS__)
#else
	#define AE_ECS_ASSERT(exp, ...)
#endif

#ifdef AE_DATASTRUCT_ASSERTS
	#define AE_DATASTRUCT_ASSERT(exp, ...) AE_CORE_ASSERT(exp, __VA_ARGS__)
#else
	#define AE_DATASTRUCT_ASSERT(exp, ...)
#endif

#ifdef AE_RENDER_ASSERTS
	#define AE_RENDER_ASSERT(exp, ...) AE_CORE_ASSERT(exp, __VA_ARGS__)
#else
	#define AE_RENDER_ASSERT(exp, ...)
#endif