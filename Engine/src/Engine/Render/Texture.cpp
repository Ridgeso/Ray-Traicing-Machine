#include "Texture.h"

#include "External/Render/OpenGl/OpenGlTexture.h"

namespace RT
{
	
	Local<Texture> Texture::create(const glm::ivec2 size, const ImageFormat imageFormat)
	{
		return makeLocal<OpenGl::OpenGlTexture>(size, imageFormat);
	}

	Texture::~Texture() { }

}
