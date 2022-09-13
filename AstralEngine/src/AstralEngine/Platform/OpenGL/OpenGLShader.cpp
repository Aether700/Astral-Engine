#include "aepch.h"
#include "OpenGLShader.h"
#include "AstralEngine/Renderer/RenderCommand.h"
#include "AstralEngine/Renderer/RendererInternals.h"

#include <glad/glad.h>
#include <fstream>

namespace AstralEngine
{
	#define TEXTURE_SLOTS_TOKEN "#NUM_TEXTURE_SLOTS"
	#define NUM_LIGHTS_TOKEN "#NUM_LIGHTS"

	static unsigned int StringToOpenGLType(const std::string& type)
	{
		if (type == "vertex")
		{
			return GL_VERTEX_SHADER;
		}
		else if (type == "fragment" || type == "pixel")
		{
			return GL_FRAGMENT_SHADER;
		}

		AE_CORE_ERROR("Unknown Type '%S' provided", type);
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		AE_PROFILE_FUNCTION();
		//Get name form filepath
		unsigned int lastSlash = (unsigned int)filepath.find_last_of("/\\");
		//check that there is any slash at all
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		unsigned int lastDot = (unsigned int)filepath.rfind('.');
		if (lastDot == std::string::npos)
		{
			lastDot = (unsigned int)filepath.size();
		}
		m_name = filepath.substr(lastSlash, lastDot - lastSlash);

		std::string fileContent = ReadFile(filepath);
		AUnorderedMap<unsigned int, std::string> shaderSrc = PreProcess(fileContent);
		CompileShaders(shaderSrc);
	}
	
	OpenGLShader::~OpenGLShader()
	{
		AE_PROFILE_FUNCTION();
		glDeleteProgram(m_rendererID);
	}

	void OpenGLShader::Bind() const
	{
		AE_PROFILE_FUNCTION();
		glUseProgram(m_rendererID);
	}
	void OpenGLShader::Unbind() const
	{
		AE_PROFILE_FUNCTION();
		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string& uniformName, int v)
	{
		AE_PROFILE_FUNCTION();
		int location = GetUniformLocation(uniformName);
		glUniform1i(location, v);
	}

	void OpenGLShader::SetIntArray(const std::string& uniformName, int* arr, unsigned int count)
	{
		AE_PROFILE_FUNCTION();
		int location = GetUniformLocation(uniformName);
		glUniform1iv(location, count, arr);
	}

	void OpenGLShader::SetInt2(const std::string& uniformName, const Vector2Int& v)
	{
		AE_PROFILE_FUNCTION();
		int location = GetUniformLocation(uniformName);
		glUniform2i(location, v.x, v.y);
	}

	void OpenGLShader::SetInt3(const std::string& uniformName, const Vector3Int& v)
	{
		AE_PROFILE_FUNCTION();
		int location = GetUniformLocation(uniformName);
		glUniform3i(location, v.x, v.y, v.z);
	}

	void OpenGLShader::SetInt4(const std::string& uniformName, const Vector4Int& v)
	{
		AE_PROFILE_FUNCTION();
		int location = GetUniformLocation(uniformName);
		glUniform4i(location, v.x, v.y, v.z, v.w);
	}


	void OpenGLShader::SetFloat(const std::string& uniformName, float v)
	{
		AE_PROFILE_FUNCTION();
		int location = GetUniformLocation(uniformName);
		glUniform1f(location, v);
	}

	void OpenGLShader::SetFloat2(const std::string& uniformName, const Vector2& v)
	{
		AE_PROFILE_FUNCTION();
		int location = GetUniformLocation(uniformName);
		glUniform2f(location, v.x, v.y);
	}

	void OpenGLShader::SetFloat3(const std::string& uniformName, const Vector3& v)
	{
		AE_PROFILE_FUNCTION();
		int location = GetUniformLocation(uniformName);
		glUniform3f(location, v.x, v.y, v.z);
	}

	void OpenGLShader::SetFloat4(const std::string& uniformName, const Vector4& v)
	{
		AE_PROFILE_FUNCTION();
		int location = GetUniformLocation(uniformName);
		glUniform4f(location, v.x, v.y, v.z, v.w);
	}


	void OpenGLShader::SetMat3(const std::string& uniformName, const Mat3& m)
	{
		AE_PROFILE_FUNCTION();
		int location = GetUniformLocation(uniformName);
		glUniformMatrix3fv(location, 1, GL_FALSE, (float*)m.Data());
	}

	void OpenGLShader::SetMat4(const std::string& uniformName, const Mat4& m)
	{
		AE_PROFILE_FUNCTION();
		int location = GetUniformLocation(uniformName);
		glUniformMatrix4fv(location, 1, GL_FALSE, (float*)m.Data());
	}

	void OpenGLShader::SetBool(const std::string& uniformName, bool v)
	{
		AE_PROFILE_FUNCTION();
		int location = GetUniformLocation(uniformName);
		glUniform1ui(location, v);
	}


	unsigned int OpenGLShader::CreateGLShader(const std::string& src, unsigned int type) //type is an GL Enum
	{
		AE_PROFILE_FUNCTION();

		unsigned int shader = glCreateShader(type);
		const char* cStr = src.c_str();
		glShaderSource(shader, 1, &cStr, 0);

		glCompileShader(shader);

		int isCompiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			int maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<char> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(shader);

			AE_CORE_ERROR("Shader Compilation Failure: %s", infoLog.data());
		}
		return shader;
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		AE_PROFILE_FUNCTION();
		std::string src;
		std::ifstream file(filepath, std::ios::in | std::ios::binary);

		if (file)
		{
			//go through file and reserve the space needed for the src string
			file.seekg(0, std::ios::end);
			src.resize(file.tellg());

			//return file to beginning and read/store the content inside the src string
			file.seekg(0, std::ios::beg);
			file.read(&src[0], src.size());
			
			file.close();
		}
		else
		{
			AE_CORE_ERROR("Could not open file '%S'", filepath);
		}

		return src;
	}

	AUnorderedMap<unsigned int, std::string> OpenGLShader::PreProcess(const std::string& src)
	{
		AE_PROFILE_FUNCTION();
		AUnorderedMap<unsigned int, std::string> shaderSrc;
		
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);

		size_t position = src.find(typeToken, 0);
		while (position != std::string::npos)
		{
			unsigned int eol = (unsigned int)src.find_first_of("\r\n", position);
			AE_CORE_ASSERT((eol != std::string::npos), "Syntax Error");
			size_t start = position + typeTokenLength + 1;

			std::string typeStr = src.substr(start, eol - start);

			unsigned int type = StringToOpenGLType(typeStr);

			size_t nextLinePos = src.find_first_not_of("\r\n", eol);
			position = src.find(typeToken, nextLinePos);

			size_t count;
			if (nextLinePos == std::string::npos)
			{
				count = position - src.size() - 1;
			}
			else
			{
				count = position - nextLinePos;
			}
			
			shaderSrc[type] = src.substr(nextLinePos, count);
		}


		std::string numLights = std::to_string(LightHandler::GetMaxNumLights());
		std::string numTextureSlots = std::to_string(RenderCommand::GetNumTextureSlots());

		for (auto& pair : shaderSrc)
		{
			std::string& srcCode = pair.GetElement();

			//size_t textureSlotTokenPos = srcCode.find(TEXTURE_SLOTS_TOKEN);
			//size_t numLightsTokenPos = srcCode.find(NUM_LIGHTS_TOKEN);

			/*
			if (textureSlotTokenPos != std::string::npos)
			{
				constexpr size_t textureSlotTokenLength = sizeof(TEXTURE_SLOTS_TOKEN) / sizeof(char);
				size_t numTextureSlots = RenderCommand::GetNumTextureSlots();
				size_t position = 0;
				std::stringstream ss;
				while (textureSlotTokenPos != std::string::npos)
				{
					ss << srcCode.substr(position, textureSlotTokenPos - position);
					ss << numTextureSlots;
					position = textureSlotTokenPos + textureSlotTokenLength - 1;
					textureSlotTokenPos = srcCode.find(TEXTURE_SLOTS_TOKEN, position);
				}

				ss << srcCode.substr(position, srcCode.length() - position);
				shaderSrc[pair.GetKey()] = ss.str();
			}
			*/

			PreprocessToken(srcCode, TEXTURE_SLOTS_TOKEN, sizeof(TEXTURE_SLOTS_TOKEN) / sizeof(char), numTextureSlots);
			PreprocessToken(srcCode, NUM_LIGHTS_TOKEN, sizeof(NUM_LIGHTS_TOKEN) / sizeof(char), numLights);
		}


		
		return shaderSrc;
	}

	void OpenGLShader::PreprocessToken(std::string& src, const char* token, size_t tokenLen,
		const std::string& tokenReplacement)
	{
		size_t tokenPos = src.find(token);

		if (tokenPos != std::string::npos)
		{
			size_t position = 0;
			std::stringstream ss;
			while (tokenPos != std::string::npos)
			{
				ss << src.substr(position, tokenPos - position);
				ss << tokenReplacement;
				position = tokenPos + tokenLen - 1;
				tokenPos = src.find(token, position);
			}

			ss << src.substr(position, src.length() - position);
			src = ss.str();
		}
	}

	void OpenGLShader::CompileShaders(AUnorderedMap<unsigned int, std::string>& shaderSrcs)
	{
		AE_PROFILE_FUNCTION();
		m_rendererID = glCreateProgram();

		unsigned int* shaders = new unsigned int[shaderSrcs.GetCount()];
		int index = 0;

		for (auto& pair : shaderSrcs)
		{
			unsigned int type = pair.GetKey();
			std::string& src = pair.GetElement();

			shaders[index] = CreateGLShader(src, type);
			index++;
		}

		for (index = 0; index < shaderSrcs.GetCount(); index++)
		{
			glAttachShader(m_rendererID, shaders[index]);
		}

		glLinkProgram(m_rendererID);

		int isLinked;
		glGetProgramiv(m_rendererID, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			int maxLength = 0;
			glGetProgramiv(m_rendererID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<char> infoLog(maxLength);
			glGetProgramInfoLog(m_rendererID, maxLength, &maxLength, &infoLog[0]);

			AE_ERROR(infoLog.data());
			AE_ASSERT(false, "Shader Linking Failure");
		}

		for (index = 0; index < shaderSrcs.GetCount(); index++)
		{
			glDetachShader(m_rendererID, shaders[index]);
		}

		delete[] shaders;
	}

	int OpenGLShader::GetUniformLocation(const std::string& name)
	{
		AE_PROFILE_FUNCTION();
		if (m_uniforms.ContainsKey(name))
		{
			return m_uniforms[name];
		}

		int location = glGetUniformLocation(m_rendererID, name.c_str());

		if (location == -1)
		{
			AE_CORE_WARN("Unknown uniform \"%s\"", name.c_str());
			return -1;
		}

		m_uniforms[name] = location;
		return location;
	}

}