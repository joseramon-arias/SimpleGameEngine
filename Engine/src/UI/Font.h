#pragma once
#include <string>
#include <unordered_map>
#include <SDL_ttf.h>
#include "CustomMath.h"

namespace Engine
{
	class Font
	{
	public:
		Font(class Game* game);
		~Font();

		// Load/unload from a file
		bool Load(const std::string& fileName);
		void Unload();

		// Given string and this font, draw to a texture
		class Texture* RenderText(const std::string& textKey,
			const Vector3& color = Color::White,
			int pointSize = 30);
	private:
		// Map of point sizes to font data
		std::unordered_map<int, TTF_Font*> m_FontData;
		class Game* m_Game;
	};
}