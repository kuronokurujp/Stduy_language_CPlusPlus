#include "Component/animaterSpriteComponent.h"

#include "3D/shader.h"
#include "texture.h"

/// <summary>
/// Initializes a new instance of the <see cref="AnimaterSpriteComponent"/> class.
/// </summary>
/// <param name="in_pActor">The in p actor.</param>
/// <param name="in_orderUpdate">The in order update.</param>

AnimaterSpriteComponent::AnimaterSpriteComponent(class Actor* in_pActor, const int in_drawUpdate, Shader* in_pShader) :
	SpriteComponent(in_pActor, in_drawUpdate, in_pShader)
{
	this->_Clear();
}

/// <summary>
/// Updates the specified in delta time.
/// </summary>
/// <param name="in_deltaTime">The in delta time.</param>
/// <returns></returns>
auto AnimaterSpriteComponent::Update(const float in_deltaTime) -> void
{
	if (this->textures.size() <= 0)
	{
		return;
	}

	this->currentFrame += this->animaterFPS * in_deltaTime;
	// 登録したテクスチャー個数を超えてしまうとテクスチャーの差し替えが出来ないので
	// 個数を超えないように補正する
	while (this->currentFrame >= this->textures.size())
	{
		this->currentFrame -= this->textures.size();
	}

	int textureIndex = static_cast<int>(this->currentFrame);
	this->SetTexture(this->textures[textureIndex]);
}

/// <summary>
/// Sets the animater textures.
/// </summary>
/// <param name="in_rTextures">The in r textures.</param>
/// <returns></returns>
auto AnimaterSpriteComponent::SetAnimaterTextures(const std::vector<Texture*>& in_rTextures) -> void
{
	this->textures = in_rTextures;
	if (this->textures.size() <= 0)
	{
		return;
	}

	this->currentFrame = 0.0f;
	this->SetTexture(this->textures[0]);
}