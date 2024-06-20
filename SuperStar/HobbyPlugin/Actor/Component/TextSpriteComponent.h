#ifndef __TEXT_SPRITE_COMPONENT_H__
#define __TEXT_SPRITE_COMPONENT_H__

#include <string.h>

#include "Component/spriteComponent.h"
#include "SDL/SDL_ttf.h"

class Actor;
class Shader;

class TextSpriteComponent : public SpriteComponent
{
public:
    /// <summary>
    /// Initializes a new instance of the <see cref="BGComponent" /> class.
    /// </summary>
    /// <param name="in_pOwner">The in p owner.</param>
    /// <param name="in_drawOrder">The in draw order.</param>
    /// <param name="in_fontSize">Size of the in font.</param>
    TextSpriteComponent(Actor* in_pOwner, const int in_drawOrder, const int in_fontSize,
                        Shader* in_pShader);

    /// <summary>
    /// Finalizes an instance of the <see cref="TextSpriteComponent"/> class.
    /// </summary>
    virtual ~TextSpriteComponent();

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
    /// Sets the text.
    /// </summary>
    /// <param name="in_pText">The in p text.</param>
    /// <param name="in_color">Color of the in.</param>
    /// <returns></returns>
    void SetText(const char* in_pText, SDL_Color in_color);

    /// <summary>
    /// Sets the color.
    /// </summary>
    /// <param name="in_color">Color of the in.</param>
    /// <returns></returns>
    void SetColor(SDL_Color in_color);

    /// <summary>
    /// Gets the size of the font.
    /// </summary>
    /// <returns></returns>
    const int GetFontSize() { return this->fontSize; }

private:
    /// <summary>
    /// Clears this instance.
    /// </summary>
    /// <returns></returns>
    void _Clear()
    {
        memset(this->textButter, 0, sizeof(this->textButter));
        this->pFontData = nullptr;
        this->pSurface  = nullptr;
        this->fontSize  = 0;
    }

    // テキスト長を固定長にする
    char textButter[256];

    TTF_Font* pFontData;
    SDL_Surface* pSurface;
    int fontSize;
};

#endif  // __TEXT_SPRITE_COMPONENT_H__