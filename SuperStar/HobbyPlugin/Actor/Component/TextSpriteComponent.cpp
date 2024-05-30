#include "Component/textSpriteComponent.h"

#include "3D/shader.h"

#include "Math/rect2.h"

#include "Actor/actor.h"

/// <summary>
/// Initializes a new instance of the <see cref="BGComponent" /> class.
/// </summary>
/// <param name="in_pOwner">The in p owner.</param>
/// <param name="in_drawOrder">The in draw order.</param>
/// <param name="in_fontSize">Size of the in font.</param>
TextSpriteComponent::TextSpriteComponent(Actor* in_pOwner, const int /*in_drawOrder*/, const int in_fontSize, Shader* in_pShader) :
	SpriteComponent(in_pOwner, 100, in_pShader)
{
	this->_Clear();

	// フォントデータのロード
	{
		this->pFontData = TTF_OpenFont("Resources/Font/PixelMplus12-Regular.ttf", in_fontSize);
		if (this->pFontData == nullptr)
		{
			SDL_Log("Unable to initialize SDK : %s", SDL_GetError());
		}
	}
}

/// <summary>
/// Finalizes an instance of the <see cref="TextSpriteComponent"/> class.
/// </summary>
TextSpriteComponent::~TextSpriteComponent()
{
	if (this->pSurface != nullptr)
	{
		SDL_FreeSurface(this->pSurface);
		this->pSurface = nullptr;
	}

	if (this->pFontData != nullptr)
	{
		TTF_CloseFont(this->pFontData);
		this->pFontData = nullptr;
	}
}

/// <summary>
/// Updates the specified in delta time.
/// </summary>
/// <param name="in_deltaTime">The in delta time.</param>
/// <returns></returns>
auto TextSpriteComponent::Update(const float /*in_deltaTime*/) -> void
{
}

/// <summary>
/// Sets the text.
/// </summary>
/// <param name="in_pText">The in p text.</param>
/// <returns></returns>
auto TextSpriteComponent::SetText(const char* in_pText, SDL_Color in_color) -> void
{
	SDL_assert(in_pText != nullptr);

	int textMaxLength = sizeof(this->textButter) / sizeof(this->textButter[0]);
	int textNowLength = strnlen_s(in_pText, textMaxLength);
	memcpy(this->textButter, in_pText, textNowLength * sizeof(in_pText[0]));

	if (this->pSurface != nullptr)
	{
		SDL_FreeSurface(this->pSurface);
		this->pSurface = nullptr;
	}

	this->pSurface = TTF_RenderUTF8_Blended(this->pFontData, this->textButter, in_color);
	SDL_assert(this->pSurface);
}

/// <summary>
/// Sets the color.
/// </summary>
/// <param name="in_color">Color of the in.</param>
/// <returns></returns>
auto TextSpriteComponent::SetColor(SDL_Color in_color) -> void
{
	this->SetText(this->textButter, in_color);
}

/// <summary>
/// Draws the specified in p renderer.
/// </summary>
/// <param name="in_pRenderer">The in p renderer.</param>
/// <returns></returns>
auto TextSpriteComponent::Draw() -> void
{
#if 0
	//SDL_Rect sdlRect;
//	Math::Rect2 r;
	//const Math::Vector2& rPosition = this->pOwner->GetPosition();

	//surfaceからTextureを作る
	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(in_pRenderer, this->pSurface);
	this->SetTexture(pTexture);

	SpriteComponent::Draw(in_pRenderer);

	SDL_DestroyTexture(pTexture);
#endif
}