#pragma once


#ifdef AE_PLATFORM_WINDOWS
	#include "AstralEngine/Platform/Windows/WindowsWindow.h"
	#include "AstralEngine/Platform/Windows/WindowsTime.h"
#endif // AE_PLATFORM_WINDOWS

// Core /////////////////////////////////////////////////////////////////
#include "aepch.h"
#include "AstralEngine/Core/Log.h"
#include "AstralEngine/Core/Application.h"
#include "AstralEngine/Core/Layer.h"
#include "AstralEngine/Core/AWindow.h"
#include "AstralEngine/Core/Input.h"
#include "AstralEngine/Core/Time.h"
#include "AstralEngine/Core/Keycodes.h"
#include "AstralEngine/Core/MouseButtonCodes.h"

// Data Structures //////////////////////////////////////////////////////

#include "AstralEngine/Data Struct/ADoublyLinkedList.h"
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/Data Struct/AHeap.h"
#include "AstralEngine/Data Struct/AKeyElementPair.h"
#include "AstralEngine/Data Struct/APriorityQueue.h"
#include "AstralEngine/Data Struct/AReference.h"
#include "AstralEngine/Data Struct/ASinglyLinkedList.h"
#include "AstralEngine/Data Struct/AStack.h"
#include "AstralEngine/Data Struct/AUniqueRef.h"
#include "AstralEngine/Data Struct/AUnorderedMap.h"
#include "AstralEngine/Data Struct/AWeakRef.h"
#include "AstralEngine/Data Struct/ADelegate.h"

// Algorithms ///////////////////////////////////////////////////////////
#include "AstralEngine/Algorithms/Algorithms.h"

// Renderer /////////////////////////////////////////////////////////////
#include "AstralEngine/Renderer/GraphicsContext.h"
#include "AstralEngine/Renderer/VertexBuffer.h"
#include "AstralEngine/Renderer/IndexBuffer.h"
#include "AstralEngine/Renderer/Framebuffer.h"
#include "AstralEngine/Renderer/Shader.h"
#include "AstralEngine/Renderer/RenderAPI.h"
#include "AstralEngine/Renderer/RenderCommand.h"
#include "AstralEngine/Renderer/Renderer.h"
#include "AstralEngine/Renderer/Renderer2D.h"
#include "AstralEngine/Renderer/OrthographicCamera.h"
#include "AstralEngine/Renderer/OrthographicCameraController.h"
#include "AstralEngine/Renderer/Texture.h"

// ECS ////////////////////////////////////////////////////////////////
#include "AstralEngine/ECS/AEntity.h"
#include "AstralEngine/ECS/Scene.h"
#include "AstralEngine/ECS/Components.h"

// Math ///////////////////////////////////////////////////////////////
#include "AstralEngine/Math/AMath.h"