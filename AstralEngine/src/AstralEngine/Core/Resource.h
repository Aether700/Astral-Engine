#pragma once
#include "AstralEngine/Data Struct/ADynArr.h"
#include "AstralEngine/Data Struct/AStack.h"
#include "AstralEngine/Data Struct/AReference.h"
#include "AstralEngine/Math/AMath.h"

namespace AstralEngine
{
	typedef size_t ResourceHandle;

	typedef ResourceHandle Texture2DHandle;
	typedef ResourceHandle ShaderHandle;
	typedef ResourceHandle MaterialHandle;
	typedef ResourceHandle MeshHandle;

	static constexpr ResourceHandle NullHandle = MAXSIZE_T;

	class Texture2D;
	class Shader;
	class Material;
	class Mesh;
	enum class Texture2DInternalFormat;

	template<typename T>
	class ResourcePool
	{
	public:
		ResourceHandle AddResource(const AReference<T>& resource)
		{
			ResourceHandle handle;
			if (m_handlesToReuse.IsEmpty())
			{
				handle = m_resourceList.GetCount();
				m_resourceList.Add(resource);
			}
			else
			{
				handle = m_handlesToReuse.Pop();
				m_resourceList[handle] = resource;
			}
			return handle;
		}

		void RemoveHandle(ResourceHandle handle)
		{
			AE_CORE_ASSERT(HandleIsValid(handle), "");
			m_handlesToReuse.Push(handle);
		}

		AReference<T>& GetResource(ResourceHandle handle)
		{
			AE_CORE_ASSERT(HandleIsValid(handle), "Invalid handle provided");
			return m_resourceList[handle];
		}

		bool HandleIsValid(ResourceHandle handle)
		{
			return handle < m_resourceList.GetCount() && !m_handlesToReuse.Contains(handle);
		}

	private:
		ADynArr<AReference<T>> m_resourceList;
		AStack<ResourceHandle> m_handlesToReuse;
	};

	class ResourceHandler
	{
	public:
		// Shader ////////////////////////////////////////////////////
		static ShaderHandle LoadShader(const std::string& filepath);
		static AReference<Shader> GetShader(ShaderHandle handle);
		static void DeleteShader(ShaderHandle handle);

		// Texture2D /////////////////////////////////////////////////
		static Texture2DHandle LoadTexture2D(const std::string& filepath);
		static Texture2DHandle CreateTexture2D(unsigned int width, unsigned int height);
		static Texture2DHandle CreateTexture2D(unsigned int width, unsigned int height, 
			Texture2DInternalFormat internalFormat);
		static Texture2DHandle CreateTexture2D(unsigned int width, unsigned int height, void* data, unsigned int size);
		static AReference<Texture2D> GetTexture2D(Texture2DHandle handle);
		static void DeleteTexture2D(Texture2DHandle handle);

		// Material /////////////////////////////////////////////////////
		static MaterialHandle CreateMaterial();
		static MaterialHandle CreateMaterial(const Vector4& color);
		static AReference<Material> GetMaterial(MaterialHandle handle);
		static void DeleteMaterial(MaterialHandle handle);

		// Mesh //////////////////////////////////////////////////////////
		static MeshHandle LoadMesh(const std::string& filepath);
		static MeshHandle CreateMesh(const ADynArr<Vector3>& positions, const ADynArr<Vector2>& textureCoords,
			const ADynArr<Vector3>& normals, const ADynArr<unsigned int>& indices);
		static AReference<Mesh> GetMesh(MeshHandle handle);
		static void DeleteMesh(MeshHandle handle);

	private:
		static ResourceHandler* GetHandler();

		ResourcePool<Texture2D> m_textures2D;
		ResourcePool<Shader> m_shaders;
		ResourcePool<Material> m_materials;
		ResourcePool<Mesh> m_meshes;

	};
}