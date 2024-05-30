#ifndef __BGCOMPONENT_H__
#define __BGCOMPONENT_H__

#include "Component/spriteComponent.h"
#include "Math/vector2.h"

#include <vector>

class Actor;
class Shader;

class BGSpriteComponent : public SpriteComponent
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="BGComponent"/> class.
	/// </summary>
	/// <param name="in_pOwner">The in p owner.</param>
	/// <param name="in_drawOrder">The in draw order.</param>
	BGSpriteComponent(Actor* in_pOwner, const int in_drawOrder, Shader* in_pShader);

	/// <summary>
	/// Updates the specified in delta time.
	/// </summary>
	/// <param name="in_deltaTime">The in delta time.</param>
	/// <returns></returns>
	void Update(const float in_deltaTime) override final;

	/// <summary>
	/// Draws the specified in p renderer.
	/// </summary>
	/// <param name="in_pRenderer">The in p renderer.</param>
	/// <returns></returns>
	void Draw() override final;

	/// <summary>
	/// Sets the bg texture.
	/// </summary>
	/// <param name="in_textures">The in textures.</param>
	/// <returns></returns>
	void SetBGTexture(const std::vector<Texture*>& in_rTextures);

	void SetScreenSize(const Math::Vector2& in_rSize)
	{
		this->screenSize = in_rSize;
	}

	void SetScrollSpeed(const float in_speed)
	{
		this->scrollSpeed = in_speed;
	}

	const float GetScrollSpeed() const { return this->scrollSpeed; }

private:
	/// <summary>
	/// テクスチャーデータ
	/// </summary>
	struct BGTexture
	{
		Texture* pTexture;
		Math::Vector2 offset;
	};

	/// <summary>
	/// Clears this instance.
	/// </summary>
	/// <returns></returns>
	void _Clear()
	{
		this->scrollSpeed = 0.0f;
		this->bgTextures.clear();
	}

	std::vector<BGTexture> bgTextures;
	Math::Vector2 screenSize;
	float scrollSpeed;
};

#endif // !__BGCOMPONENT_H__
