#pragma once
#include "AstralEngine/Renderer/Texture.h"

namespace AstralEngine
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(unsigned int width, unsigned int height);
		OpenGLTexture2D(unsigned int width, unsigned int height, Texture2DInternalFormat internalFormat);
		OpenGLTexture2D(unsigned int width, unsigned int height, void* data, unsigned int size);
		OpenGLTexture2D(const std::string& path);
		~OpenGLTexture2D();

		virtual unsigned int GetWidth() const override;
		virtual unsigned int GetHeight() const override;
		virtual unsigned int GetTextureID() const override;

		virtual void SetData(void* data, unsigned int size) override;

		virtual void Bind(unsigned int slot = 0) const override;

		virtual Texture2DInternalFormat GetInternalFormat() const override;

		virtual bool operator==(const Texture& other) const override;

	private:
		unsigned int m_rendererID;
		unsigned int m_width;
		unsigned int m_height;
		Texture2DInternalFormat m_internalFormat;
		unsigned int m_dataFormat;
	};

	//cube map used to generate textures for the voxels produced by the batch renderer
	class OpenGLCubeMap : public CubeMap
	{
		friend class Renderer3D;
	public:

		/*pass the texture used for each face where each texture
		  is applied in this order

		  index 0 -> right face (positive X)
		  index 1 -> left face (negative X)
		  index 2 -> top face (positive Y)
		  index 3 -> bottom face (negative Y)
		  index 4 -> front face (positive Z)
		  index 5 -> back face (negative Z)
		*/
		OpenGLCubeMap(const std::array<std::string, 6>& faceTextures);

		//will apply the same texture to every face
		OpenGLCubeMap(const std::string& faceTextures);

		//applies the texture data provided to all sides of the cube
		OpenGLCubeMap(unsigned int width, void* data);

		~OpenGLCubeMap();

		virtual unsigned int GetTextureID() const { return m_rendererID; }

		virtual void SetData(void* data, unsigned int size) override;

		//binds the cubemap to the provided texture slot (defaults to slot 0)
		virtual void Bind(unsigned int slot = 0) const override;

		unsigned int GetWidth() const override { return m_size; }
		unsigned int GetHeight() const override { return m_size; }


		//check if this cubemap is equal to the provided texture
		virtual bool operator==(const Texture& other) const override
		{
			const OpenGLCubeMap* ptr = dynamic_cast<const OpenGLCubeMap*>(&other);

			if (ptr == nullptr)
			{
				return false;
			}
			return ptr->m_rendererID == m_rendererID;
		}

	private:

		unsigned int m_rendererID;

		//all textures need to have the same width and height
		unsigned int m_size;

		//format for each of the different faces, indices are matched with 
		//the order in which the faces are passed to the constructor
		std::array<unsigned int, 6> m_internalFormat, m_dataFormat;
	};

}