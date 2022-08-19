#pragma once
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/Data Struct/AReference.h"

namespace AstralEngine
{
	typedef size_t ResourceHandle;

	typedef ResourceHandle Texture2DHandle;
	typedef ResourceHandle ShaderHandle;
	typedef ResourceHandle MaterialHandle;

	class Texture2D;
	class Shader;
	class Material;

	template<typename T>
	class ResourcePool
	{
	public:
		ResourceHandle AddResource(const AReference<T>& resource)
		{
			ResourceHandle handle = m_resourceList.GetCount();
			m_resourceList.Add(resource);
			return handle;
		}

		AReference<T>& GetResource(ResourceHandle handle)
		{
			AE_CORE_ASSERT(HandleIsValid(handle), "Invalid handle provided");
			return m_resourceList[handle];
		}

		bool HandleIsValid(ResourceHandle handle)
		{
			return handle < m_resourceList.GetCount();
		}

	private:
		ADynArr<AReference<T>> m_resourceList;
	};

	class ResourceHandler
	{
	public:
		// Shader ////////////////////////////////////////////////////
		static ShaderHandle LoadShader(const std::string& filepath);
		static AReference<Shader> GetShader(ShaderHandle handle);

		// Texture2D /////////////////////////////////////////////////
		static Texture2DHandle LoadTexture2D(const std::string& filepath);
		static Texture2DHandle LoadTexture2D(unsigned int width, unsigned int height);
		static Texture2DHandle LoadTexture2D(unsigned int width, unsigned int height, void* data, unsigned int size);
		static AReference<Texture2D> GetTexture2D(Texture2DHandle handle);

		static MaterialHandle CreateMaterial();
		static MaterialHandle CreateMaterial(const Vector4& color);
		static AReference<Material> GetMaterial(MaterialHandle handle);

	private:
		static ResourceHandler* GetHandler();

		ResourcePool<Texture2D> m_textures2D;
		ResourcePool<Shader> m_shaders;
		ResourcePool<Material> m_materials;

	};
}