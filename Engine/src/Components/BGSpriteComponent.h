#pragma once

#include "SpriteComponent.h"
#include <vector>
#include "CustomMath.h"

namespace Engine
{
	class BGSpriteComponent : public SpriteComponent
	{
	public:
		BGSpriteComponent(class Actor* owner, int drawOrder = 10);
		void Update(float deltaTime) override;
		void Draw(Shader* shader) override;
		void SetBGTextures(const std::vector<class Texture*>& textures);
		void SetScreenSize(const Vector2 size) { m_ScreenSize = size; }
		void SetScrollSpeed(float speed) { m_ScrollSpeed = speed; }
		float GetScrollSpeed() const { return m_ScrollSpeed; }
	private:
		struct BGTexture
		{
			class Texture* m_Texture;
			Vector2 m_Offset;
		};
		std::vector<BGTexture> m_BGTextures;
		Vector2 m_ScreenSize;
		float m_ScrollSpeed;
	};

}