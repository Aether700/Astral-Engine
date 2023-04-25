#pragma once
#include "AstralEngine/Renderer/GraphicsContext.h"

class AWindow;

#include <wingdi.h>

namespace AstralEngine
{
#ifdef AE_PLATFORM_WINDOWS
	typedef HGLRC NativeOpenGLContext;
#else
	#error no platform macro defined
#endif

	class OpenGLGraphicsContext : public GraphicsContext
	{
	public:
		OpenGLGraphicsContext(AWindow* window);
		~OpenGLGraphicsContext();

		void Init() override;
		void SwapBuffers() override;

	private:
		NativeOpenGLContext CreateNativeContext(AWindow* window) const;
		void MakeContextCurrent() const;

		AWindow* m_window;
		NativeOpenGLContext m_nativeContext;
	};
}