#pragma once
#include <cstdint>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Engine/Render/Shader.h"

namespace RT::OpenGl
{

	struct Uniform
	{
		enum Type
		{
			None,
			Int, Int2, Int3, Int4, IntV,
			Uint, Uint2, Uint3, Uint4, UintV,
			Float, Float2, Float3, Float4, FloatV,
			Double, Double2, Double3, Double4, DoubleV,
			Mat2x2, Mat3x3, Mat4x4,
			Buffer
		};

		Type type;
		int32_t id;
	};

	class OpenGlShader : public Shader
	{
	public:
		enum Type
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

		void use() const final;
		void unuse() const final;
		void destroy() final;
		const uint32_t getId() const final { return programId; }

		void load(const std::string& shaderPath) final;
		
		void setUniformImpl(const std::string& uniName, const int32_t size, const void* value) const final;

	private:
		void setStorage(const int32_t pos, const size_t size, const void* data) const;
		std::unordered_map<Type, std::stringstream> readSources(const std::string& shaderPath) const;
		uint32_t compile(const Type type, const std::string& source) const;
		void loadUniforms();
		void logLinkError(const std::string& shaderPath) const;

		static constexpr uint32_t shaderType2GlType(const Type type);
		static constexpr Uniform::Type glUniType2UniType(const uint32_t glUniType);

	private:
		uint32_t programId;
		std::unordered_map<std::string, Uniform> uniforms;
		std::unordered_map<int32_t, uint32_t> storages;
	};

}
