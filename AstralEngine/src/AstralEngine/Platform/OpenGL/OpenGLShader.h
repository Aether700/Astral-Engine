#pragma once
#include "AstralEngine/Renderer/Shader.h"
#include "AstralEngine/Data Struct/AUnorderedMap.h"
#include "AstralEngine/Data Struct/AReference.h"

namespace AstralEngine
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		~OpenGLShader();

		virtual const std::string& GetName() const override { return m_name; }

		virtual unsigned int GetRendererID() const override { return m_rendererID; }

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string& uniformName, int v) override;
		virtual void SetIntArray(const std::string& uniformName, int* arr, unsigned int count) override;
		virtual void SetInt2(const std::string& uniformName, const Vector2Int& v) override;
		virtual void SetInt3(const std::string& uniformName, const Vector3Int& v) override;
		virtual void SetInt4(const std::string& uniformName, const Vector4Int& v) override;

		virtual void SetFloat(const std::string& uniformName, float v) override;
		virtual void SetFloat2(const std::string& uniformName, const Vector2& v) override;
		virtual void SetFloat3(const std::string& uniformName, const Vector3& v) override;
		virtual void SetFloat4(const std::string& uniformName, const Vector4& v) override;

		virtual void SetMat3(const std::string& uniformName, const Mat3& m) override;
		virtual void SetMat4(const std::string& uniformName, const Mat4& m) override;

		virtual void SetBool(const std::string& uniformName, bool v) override;


	private:
		int GetUniformLocation(const std::string& name);
		std::string ReadFile(const std::string& filepath);
		

		//type is a GLenum
		unsigned int CreateGLShader(const std::string& src, unsigned int type);
		AUnorderedMap<unsigned int, std::string> PreProcess(const std::string& src);
		void PreprocessToken(std::string& src, const char* token, size_t tokenLen, 
			const std::string& tokenReplacement);
		void CompileShaders(AUnorderedMap<unsigned int, std::string>& shaderSrcs);

		unsigned int m_rendererID;
		std::string m_name;
		AUnorderedMap<std::string, int> m_uniforms;
	};
}