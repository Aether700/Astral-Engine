#pragma once
#include "AstralEngine/Data Struct/AReference.h"
#include <string>

namespace AstralEngine
{
	class Texture
	{
	public:
		virtual ~Texture() { }

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual unsigned int GetTextureID() const = 0;


		virtual void SetData(void* data, unsigned int size) = 0;

		virtual void Bind(unsigned int slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() { }

		static AReference<Texture2D> Create(const std::string& path);
		static AReference<Texture2D> Create(unsigned int width, unsigned int height);
		static AReference<Texture2D> Create(unsigned int width, unsigned int height, void* data, unsigned int size);
	};
}