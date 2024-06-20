#ifndef __ANIMATER_COMPONENT_H__
#define __ANIMATER_COMPONENT_H__

#include <vector>

#include "Engine/Headers/Component/spriteComponent.h"

class Actor;
class Shader;
class Texture;

/// <summary>
/// アニメーションスプライトコンポーネント
/// </summary>
/// <seealso cref="SpriteComponent" />
class AnimaterSpriteComponent : public SpriteComponent
{
public:
    /// <summary>
    /// Initializes a new instance of the <see cref="AnimaterSpriteComponent" /> class.
    /// </summary>
    /// <param name="in_pActor">The in p actor.</param>
    /// <param name="in_drawUpdate">The in draw update.</param>
    /// <param name="in_pShader">The in p shader.</param>
    AnimaterSpriteComponent(Actor* in_pActor, const int in_drawUpdate, Shader* in_pShader);

    /// <summary>
    /// Updates the specified in delta time.
    /// </summary>
    /// <param name="in_deltaTime">The in delta time.</param>
    /// <returns></returns>
    void Update(const float in_deltaTime) override final;

    /// <summary>
    /// Sets the animater textures.
    /// </summary>
    /// <param name="in_rTextures">The in r textures.</param>
    /// <returns></returns>
    void SetAnimaterTextures(const std::vector<Texture*>& in_rTextures);

    /// <summary>
    /// Sets the animater FPS.
    /// </summary>
    /// <param name="in_fps">The in FPS.</param>
    /// <returns></returns>
    void SetAnimaterFPS(const float in_fps) { this->animaterFPS = in_fps; }

private:
    /// <summary>
    /// Clears this instance.
    /// </summary>
    /// <returns></returns>
    void _Clear()
    {
        this->currentFrame = 0.0f;
        this->animaterFPS  = 0.0f;
        this->textures.clear();
    }

    float currentFrame;
    float animaterFPS;

    std::vector<Texture*> textures;
};

#endif  // !__ANIMATER_COMPONENT_H__
