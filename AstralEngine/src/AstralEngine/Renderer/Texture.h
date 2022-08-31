#pragma once
#include "AstralEngine/Data Struct/AReference.h"
#include "AstralEngine/Core/Resource.h"

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

	enum class Texture2DInternalFormat
	{
		Depth24Stencil8,
		RGBA8
	};

	class Texture2D : public Texture
	{
		friend class ResourceHandler;
	public:
		virtual ~Texture2D() { }
		
		static ResourceHandle WhiteTexture();

	private:
		static AReference<Texture2D> Create(const std::string& path);
		static AReference<Texture2D> Create(unsigned int width, unsigned int height);
		static AReference<Texture2D> Create(unsigned int width, unsigned int height, 
			Texture2DInternalFormat internalFormat);
		static AReference<Texture2D> Create(unsigned int width, unsigned int height, void* data, unsigned int size);
	};

	//sub texture of a texture atlas
	class SubTexture2D
	{
	public:
		//min is the top left corner of texture and max is the bottom right corner of the texture
		SubTexture2D(const AReference<Texture2D>& texture, Vector2 min, Vector2 max) 
			: m_texture(texture), m_min(min), m_max(max) { }

		const AReference<Texture2D>& GetTexture() const { return m_texture; }
		const Vector2& GetMin() const { return m_min; }
		const Vector2& GetMax() const { return m_max; }

		static AReference<SubTexture2D> CreateFromCoords(const AReference<Texture2D>& texture, 
			const Vector2& coords, const Vector2& cellSize, const Vector2& spriteSize = { 1, 1 })
		{
			//divide by width/height to get coords between 0.0f & 1.0f
			Vector2 min = { (coords.x * cellSize.x) / texture->GetWidth(), (coords.y * cellSize.y) / texture->GetHeight() };
			Vector2 max = { ((coords.x + spriteSize.x) * cellSize.x) / texture->GetWidth(),
				((coords.y + spriteSize.y) * cellSize.y) / texture->GetHeight() };

			return AReference<SubTexture2D>::Create(texture, min, max);
		}

	private:
		AReference<Texture2D> m_texture;
		Vector2 m_min; //top left corner of the texture
		Vector2 m_max; //bottom right corner of the texture
	};

	class CubeMap : public Texture
	{
	public:
		virtual ~CubeMap() { }

		/*pass the texture used for each face where each texture
		  is applied in this order

		  index 0 -> right face (positive X)
		  index 1 -> left face (negative X)
		  index 2 -> top face (positive Y)
		  index 3 -> bottom face (negative Y)
		  index 4 -> front face (positive Z)
		  index 5 -> back face (negative Z)
		*/
		static AReference<CubeMap> Create(const std::array<std::string, 6>& faceTextures);

		//will apply the same texture to every face
		static AReference<CubeMap> Create(const std::string& faceTexture);

		//applies the texture data provided to all sides of the cube
		static AReference<CubeMap> Create(unsigned int size, void* data);
	};
}