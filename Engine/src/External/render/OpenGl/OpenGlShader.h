#pragma once
#include <cstdint>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Engine/Render/Shader.h"

namespace RT::OpenGl
{

	class OpenGlShader : public Shader
	{
	public:
		enum ShaderType
		{
			None,
			Vertex,
			TessEvaulation,
			TessControl,
			Geometry,
			Fragment,
			Compute
		};

	public:
		OpenGlShader() = default;
		~OpenGlShader() = default;

		void use() const override;
		void unuse() const override;
		void destroy() override;

		void load(const std::string& shaderPath) override;

		void setInt(const std::string uniName, const int32_t value) const override;
		void setUint(const std::string uniName, const uint32_t value) const override;
		void setFloat(const std::string uniName, const float value) const override;
		void setFloat2(const std::string uniName, const glm::vec2 value) const override;
		
		void setStorage(const uint32_t storageId, const int32_t pos, const size_t size, const void* data) const override;

		const uint32_t getId() const override { return programId; }

	private:
		std::unordered_map<ShaderType, std::stringstream> readSources(const std::string& shaderPath) const;
		uint32_t compile(const ShaderType type, const std::string& source) const;
		void loadUniforms();
		void logLinkError(const std::string shaderPath) const;

		static constexpr uint32_t shaderType2GlType(const ShaderType type);

	private:
		uint32_t programId;
		std::unordered_map<std::string, uint32_t> uniforms;
	};

}
