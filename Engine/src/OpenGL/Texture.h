#pragma once

#include <string>
#include "SDL.h"

namespace Engine
{
	class Texture
	{
	public:
		Texture();
		~Texture();

		bool Load(const std::string& fileName);
		void Unload();
		void CreateFromSurface(SDL_Surface* surface);

		void SetActive();
		
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
	private:
		// OpenGL ID of this texture
		unsigned int m_TextureID;
		int m_Width;
		int m_Height;
	};
}