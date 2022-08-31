#include "aepch.h"
#include "Resource.h"
#include "AstralEngine/Renderer/Texture.h"
#include "AstralEngine/Renderer/Shader.h"
#include "AstralEngine/Renderer/Renderer.h"
#include "AstralEngine/Renderer/Mesh.h"

namespace AstralEngine
{
	// ResourceHandler /////////////////////////////////////////////////////////////
	ShaderHandle ResourceHandler::LoadShader(const std::string& filepath)
	{
		return GetHandler()->m_shaders.AddResource(Shader::Create(filepath));
	}

	AReference<Shader> ResourceHandler::GetShader(ShaderHandle handle)
	{
		return GetHandler()->m_shaders.GetResource(handle);
	}

	void ResourceHandler::DeleteShader(ShaderHandle handle)
	{
		GetHandler()->m_shaders.RemoveHandle(handle);
	}

	Texture2DHandle ResourceHandler::LoadTexture2D(const std::string& filepath)
	{
		AReference<Texture2D> texture = Texture2D::Create(filepath);
		if (texture == nullptr)
		{
			return NullHandle;
		}
		return GetHandler()->m_textures2D.AddResource(texture);
	}
	
	Texture2DHandle ResourceHandler::CreateTexture2D(unsigned int width, unsigned int height)
	{
		AReference<Texture2D> texture = Texture2D::Create(width, height);
		if (texture == nullptr)
		{
			return NullHandle;
		}
		return GetHandler()->m_textures2D.AddResource(texture);
	}

	Texture2DHandle ResourceHandler::CreateTexture2D(unsigned int width, unsigned int height,
		void* data, unsigned int size)
	{
		AReference<Texture2D> texture = Texture2D::Create(width, height, data, size);
		if (texture == nullptr)
		{
			return NullHandle;
		}
		return GetHandler()->m_textures2D.AddResource(texture);
	}

	AReference<Texture2D> ResourceHandler::GetTexture2D(Texture2DHandle handle)
	{
		return GetHandler()->m_textures2D.GetResource(handle);
	}

	void ResourceHandler::DeleteTexture2D(Texture2DHandle handle)
	{
		GetHandler()->m_textures2D.RemoveHandle(handle);
	}

	MaterialHandle ResourceHandler::CreateMaterial()
	{
		return GetHandler()->m_materials.AddResource(AReference<Material>::Create());
	}

	MaterialHandle ResourceHandler::CreateMaterial(const Vector4& color)
	{
		return GetHandler()->m_materials.AddResource(AReference<Material>::Create(color));
	}

	AReference<Material> ResourceHandler::GetMaterial(MaterialHandle handle)
	{
		AE_PROFILE_FUNCTION();
		return GetHandler()->m_materials.GetResource(handle);
	}

	void ResourceHandler::DeleteMaterial(MaterialHandle handle)
	{
		GetHandler()->m_materials.RemoveHandle(handle);
	}

	MeshHandle ResourceHandler::LoadMesh(const std::string& filepath)
	{
		AReference<Mesh> m = Mesh::LoadFromFile(filepath);
		if (m == nullptr)
		{
			return NullHandle;
		}
		return GetHandler()->m_meshes.AddResource(m);
	}

	MeshHandle ResourceHandler::CreateMesh(const ADynArr<Vector3>& positions, const ADynArr<Vector2>& textureCoords,
		const ADynArr<Vector3>& normals, const ADynArr<unsigned int>& indices)
	{
		AReference<Mesh> m = AReference<Mesh>::Create(positions, textureCoords, normals, indices);
		if (m == nullptr)
		{
			return NullHandle;
		}
		return GetHandler()->m_meshes.AddResource(m);
	}

	AReference<Mesh> ResourceHandler::GetMesh(MeshHandle handle)
	{
		AE_PROFILE_FUNCTION();
		return GetHandler()->m_meshes.GetResource(handle);
	}

	void ResourceHandler::DeleteMesh(MeshHandle handle)
	{
		GetHandler()->m_meshes.RemoveHandle(handle);
	}

	ResourceHandler* ResourceHandler::GetHandler()
	{
		static ResourceHandler* handler = new ResourceHandler();
		return handler;
	}

}