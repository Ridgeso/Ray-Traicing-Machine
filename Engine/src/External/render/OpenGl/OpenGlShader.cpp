#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "OpenGlShader.h"

namespace RT::OpenGl
{

	void OpenGlShader::use() const
	{
		glUseProgram(programId);
	}

	void OpenGlShader::unuse() const
	{
		glUseProgram(0);
	}

    void OpenGlShader::destroy()
    {
        glDeleteProgram(programId);
        uniforms.clear();
    }

	void OpenGlShader::load(const std::string& shaderPath)
	{
        auto shadersSource = readSources(shaderPath);
        if (shadersSource.size() == 0)
        {
            std::cout << "[Shader] Source code couldn't be found: " << shaderPath << std::endl;
            return;
        }

        auto shaderIds = std::vector<uint32_t>();
        programId = glCreateProgram();
        for (const auto& [type, source] : shadersSource)
        {
            auto shaderId = shaderIds.emplace_back(compile(type, source.str()));
            glAttachShader(programId, shaderId);
        }

        glLinkProgram(programId);
        logLinkError(shaderPath);

        for (auto id : shaderIds)
        {
            glDetachShader(programId, id);
            glDeleteShader(id);
        }

        loadUniforms();
	}

    void OpenGlShader::setInt(const std::string uniName, const int32_t value) const
    {
        auto pos = uniforms.find(uniName);
        if (pos != uniforms.end())
            glUniform1i(pos->second, value);
    }

    void OpenGlShader::setUint(const std::string uniName, const uint32_t value) const
    {
        auto pos = uniforms.find(uniName);
        if (pos != uniforms.end())
            glUniform1ui(pos->second, value);
    }

    void OpenGlShader::setFloat(const std::string uniName, const float value) const
    {
        auto pos = uniforms.find(uniName);
        if (pos != uniforms.end())
            glUniform1f(pos->second, value);
    }

    void OpenGlShader::setFloat2(const std::string uniName, const glm::vec2 value) const
    {
        auto pos = uniforms.find(uniName);
        if (pos != uniforms.end())
            glUniform2f(pos->second, value.x, value.y);
    }

    void OpenGlShader::setStorage(const uint32_t storageId, const int32_t pos, const size_t size, const void* data) const
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, storageId);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, pos, storageId);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    std::unordered_map<OpenGlShader::ShaderType, std::stringstream> OpenGlShader::readSources(const std::string& shaderPath) const
    {
        std::ifstream shaders(shaderPath, std::ios::in);
        if (!shaders.is_open())
        {
            return {};
        }

        auto emptyShaders = std::unordered_map<ShaderType, std::stringstream>();
        emptyShaders[ShaderType::Vertex] << "";
        emptyShaders[ShaderType::TessEvaulation] << "";
        emptyShaders[ShaderType::TessControl] << "";
        emptyShaders[ShaderType::Geometry] << "";
        emptyShaders[ShaderType::Fragment] << "";
        emptyShaders[ShaderType::Compute] << "";
        auto shadersSource = std::unordered_map<ShaderType, std::stringstream>();
        
        auto shaderType = ShaderType::None;
        auto line = std::string();
        while (std::getline(shaders, line))
        {
            for (auto& [_, source] : emptyShaders)
            {
                source << '\n';
            }

            if (line.find("###SHADER") != std::string::npos)
            {
                shaderType = line.find("VERTEX") != std::string::npos ?   ShaderType::Vertex
                    : line.find("TESS_CONTROL") != std::string::npos ?    ShaderType::TessControl
                    : line.find("TESS_EVAULATION") != std::string::npos ? ShaderType::TessEvaulation
                    : line.find("GEOMETRY") != std::string::npos ?        ShaderType::Geometry
                    : line.find("FRAGMENT") != std::string::npos ?        ShaderType::Fragment
                    : line.find("FRAGMENT") != std::string::npos ?        ShaderType::Compute
                    :                                                     ShaderType::None;

                if (emptyShaders.find(shaderType) != emptyShaders.end())
                {
                    shadersSource[shaderType] << emptyShaders[shaderType].str();
                    emptyShaders.erase(shaderType);
                }
            }
            else if (shaderType != ShaderType::None)
            {
                shadersSource[shaderType] << line << '\n';
            }
        }

        return shadersSource;
    }

    uint32_t OpenGlShader::compile(const ShaderType type, const std::string& source) const
    {
        const auto glShaderType = shaderType2GlType(type);
        auto shaderId = static_cast<uint32_t>(glCreateShader(glShaderType));

        auto sourceData = source.c_str();
        auto sourceLenght = static_cast<int32_t>(source.length());
        glShaderSource(shaderId, 1, &sourceData, &sourceLenght);
        glCompileShader(shaderId);

        return shaderId;
    }

    void OpenGlShader::loadUniforms()
    {
        constexpr size_t bufSize = 64;
        char name[bufSize];
        int32_t count, size, length;
        uint32_t type;
        glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &count);
        for (auto i = 0u; i < count; i++)
        {
            glGetActiveUniform(programId, i, bufSize, &length, &size, &type, name);
            uniforms[std::string(name)] = glGetUniformLocation(programId, name);
        }
    }

    void OpenGlShader::logLinkError(const std::string shaderPath) const
    {
        int32_t isLinked;
        glGetProgramiv(programId, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength;
            glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<char> infoLog(maxLength);
            glGetProgramInfoLog(programId, maxLength, &maxLength, infoLog.data());
            std::cout << "[Shader] Linking failed " << shaderPath << ":\n" << infoLog.data() << std::endl;

            glDeleteProgram(programId);
        }
    }

    constexpr uint32_t OpenGlShader::shaderType2GlType(const ShaderType type)
    {
        switch (type)
        {
            case ShaderType::Vertex:         return GL_VERTEX_SHADER;
            case ShaderType::TessEvaulation: return GL_TESS_EVALUATION_SHADER;
            case ShaderType::TessControl:    return GL_TESS_CONTROL_SHADER;
            case ShaderType::Geometry:       return GL_GEOMETRY_SHADER;
            case ShaderType::Fragment:       return GL_FRAGMENT_SHADER;
            case ShaderType::Compute:        return GL_COMPUTE_SHADER;
            default:                         return 0u;
        }
    }

}
