#include "AssetManger.h"
#define STBI_FAILURE_USERMSG


#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <glad/glad.h>

//gloable helper function
TextureData LoadTextureData(std::string filepath);

namespace AssetManger
{


    unsigned int LoadTexture(std::string filepath)
    {
        TextureData data = LoadTextureData(filepath);
		unsigned int textureID;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Set up the texture with RGBA format
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.m_width, data.m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.m_data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data.m_data);
		return textureID;
    }

}

TextureData LoadTextureData(std::string filepath)
{
    stbi_set_flip_vertically_on_load(false);
    TextureData textureData;
    textureData.m_data = stbi_load(filepath.data(), &textureData.m_width, &textureData.m_height, &textureData.m_numChannels, 0);
    return textureData;
}
