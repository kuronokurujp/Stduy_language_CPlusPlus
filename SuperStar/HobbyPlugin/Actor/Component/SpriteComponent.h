#ifndef __SPRITE_COMPONENT_H__
#define __SPRITE_COMPONENT_H__

#include "Component/component.h"
#include "SDL/SDL.h"

class Shader;
class Texture;
class Actor;

/// <summary>
/// 2Dスプライトのコンポーネント
/// </summary>
/// <seealso cref="Component" />
class SpriteComponent : public Component
{
public:
	/// <summary>
	/// スプライトのグラフィックデータ
	/// </summary>
	struct GraphicData
	{
		Shader* pShader;
		Texture* pTexture;

		GraphicData()
		{
			this->pShader = nullptr;
			this->pTexture = nullptr;
		}

		GraphicData(Shader* in_pShader, Texture* in_pTexture)
		{
			this->pShader = in_pShader;
			this->pTexture = in_pTexture;
		}
	};

	/// <summary>
	/// Initializes a new instance of the <see cref="SpriteComponent" /> class.
	/// </summary>
	/// <param name="in_pOwner">The in p owner.</param>
	/// <param name="in_drawOrder">The in draw order.</param>
	SpriteComponent(Actor* in_pOwner, const int in_drawOrder, GraphicData& in_rGraphicData);
	SpriteComponent(Actor* in_pOwner, const int in_drawOrder, Shader* in_pShader);

	/// <summary>
	/// Finalizes an instance of the <see cref="SpriteComponent" /> class.
	/// </summary>
	~SpriteComponent();

	/// <summary>
	/// Updates the specified in delta time.
	/// </summary>
	/// <param name="in_deltaTime">The in delta time.</param>
	void Update(const float in_deltaTime) override;

	/// <summary>
	/// Sets the texture.
	/// </summary>
	/// <param name="in_pTexture">The in p texture.</param>
	/// <returns></returns>
	void SetTexture(Texture* in_pTexture);

	/// <summary>
	/// Draws the specified in p renderer.
	/// </summary>
	/// <param name="in_pRenderer">The in p renderer.</param>
	/// <returns></returns>
	virtual void Draw();

	inline const int GetDrawOrder() const { return this->drawOrder; }

protected:

	int drawOrder;
	GraphicData grapchiData;

private:
	void _Init(const int in_drawOrder, GraphicData& in_rGraphicData);

	/// <summary>
	/// Clears this instance.
	/// </summary>
	/// <returns></returns>
	void _Clear()
	{
		this->drawOrder = 0;
	}
};

#endif // !__SPRITE_COMPONENT_H__
