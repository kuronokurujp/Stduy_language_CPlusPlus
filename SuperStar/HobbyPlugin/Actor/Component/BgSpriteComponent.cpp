#include "Component/bgSpriteComponent.h"

#include "3D/shader.h"
#include "Actor/actor.h"
#include "Math/rect2.h"

/// <summary>
/// Initializes a new instance of the <see cref="BGComponent"/> class.
/// </summary>
/// <param name="in_pOwner">The in p owner.</param>
/// <param name="in_drawOrder">The in draw order.</param>
BGSpriteComponent::BGSpriteComponent(Actor* in_pOwner, const int /*in_drawOrder*/,
                                     Shader* in_pShader)
    : SpriteComponent(in_pOwner, 100, in_pShader)
{
    this->_Clear();
}

/// <summary>
/// Updates the specified in delta time.
/// </summary>
/// <param name="in_deltaTime">The in delta time.</param>
/// <returns></returns>
auto BGSpriteComponent::Update(const float in_deltaTime) -> void
{
    for (auto& bg : this->bgTextures)
    {
        bg.offset.x += this->scrollSpeed * in_deltaTime;
        // スクリーンから横サイズ１つ分はみ出したら位置を補正する
        // 横に並べたテクスチャー数-1 * スクリーン横サイズ分で配置しなおす
        if (bg.offset.x < -this->screenSize.x)
        {
            bg.offset.x = (this->bgTextures.size() - 1) * this->screenSize.x - 1;
        }
    }
}

/// <summary>
/// Sets the bg texture.
/// </summary>
/// <param name="in_textures">The in textures.</param>
/// <returns></returns>
auto BGSpriteComponent::SetBGTexture(const std::vector<Texture*>& in_rTextures) -> void
{
    int count = 0;
    for (auto texture : in_rTextures)
    {
        BGTexture temp;
        temp.pTexture = texture;
        temp.offset.x = static_cast<float>(count) * this->screenSize.x;
        temp.offset.y = 0.0f;
        this->bgTextures.emplace_back(temp);

        ++count;
    }
}

/// <summary>
/// Draws the specified in p renderer.
/// </summary>
/// <param name="in_pRenderer">The in p renderer.</param>
/// <returns></returns>
auto BGSpriteComponent::Draw() -> void
{
#if 0
	SDL_Rect sdlRect;
	Math::Rect2 r;
	const Math::Vector2& rPosition = this->pOwner->GetPosition();

	for (auto& bg : this->bgTextures)
	{
		r = Math::Rect2(
			rPosition.x + bg.offset.x,
			rPosition.y + bg.offset.y,
			static_cast<int>(this->screenSize.x),
			static_cast<int>(this->screenSize.y)
		);

		sdlRect = Math::ConverterRectToSDLRect(r);
		SDL_RenderCopy(
			in_pRenderer,
			bg.pTexture,
			nullptr,
			&sdlRect
		);
	}
#endif
}