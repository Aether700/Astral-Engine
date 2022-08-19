#pragma once
#include "AstralEngine/Data Struct/AReference.h"
#include "AstralEngine/Data Struct/AUnorderedMap.h"
#include "AstralEngine/Math/AMath.h"

#include <string>

namespace AstralEngine
{
	class Shader
	{
		friend class ResourceHandler;
	public:
		virtual ~Shader() { }

		virtual const std::string& GetName() const = 0;

		virtual unsigned int GetRendererID() const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string& uniformName, int v) = 0;
		virtual void SetIntArray(const std::string& uniformName, int* arr, unsigned int count) = 0;
		virtual void SetInt2(const std::string& uniformName, const Vector2Int& v) = 0;
		virtual void SetInt3(const std::string& uniformName, const Vector3Int& v) = 0;
		virtual void SetInt4(const std::string& uniformName, const Vector4Int& v) = 0;

		virtual void SetFloat(const std::string& uniformName, float v) = 0;
		virtual void SetFloat2(const std::string& uniformName, const Vector2& v) = 0;
		virtual void SetFloat3(const std::string& uniformName, const Vector3& v) = 0;
		virtual void SetFloat4(const std::string& uniformName, const Vector4& v) = 0;

		virtual void SetMat3(const std::string& uniformName, const Mat3& m) = 0;
		virtual void SetMat4(const std::string& uniformName, const Mat4& m) = 0;

		virtual void SetBool(const std::string& uniformName, bool v) = 0;

	private:
		static AReference<Shader> Create(const std::string& filepath);
		static AReference<Shader> Create(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
	};
}