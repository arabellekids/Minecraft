#include <iostream>
#include <SDL2/SDL_image.h>

#include <GLES3/gl32.h>
#include "texture.h"

Texture::Texture(const std::string& path, unsigned int filterMode)
 : m_rendererID(0)
{
    SDL_Surface* tmp = IMG_Load(path.c_str());
    
    if(tmp == nullptr)
    {
        // Failed to load image
        std::cerr << "Failed to load texture at: " << path << "\n";
        return;
    }

    // Convert the loaded image to RGBA8 format
    SDL_Surface* surf = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(tmp);

    // Create an OpenGL Texture object
    glGenTextures(1, &m_rendererID);
    glBindTexture(GL_TEXTURE_2D, m_rendererID);

    // Set up the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set the texture pixels
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8, surf->w,surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Free the loaded surface
    SDL_FreeSurface(surf);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_rendererID);
}

void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_rendererID);
}

void Texture::UnBind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
