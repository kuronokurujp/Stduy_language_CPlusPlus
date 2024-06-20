#include "Component/spriteComponent.h"

#include <GL/glew.h>

#include "3D/shader.h"
#include "3D/vertexArray.h"
#include "Actor/actor.h"
#include "Math/math.h"
#include "Math/rect2.h"
#include "game.h"
#include "renderer.h"
#include "texture.h"

static VertexArray* s_pVertexArray = nullptr;

SpriteComponent::SpriteComponent(class Actor* in_pOwner, const int in_drawOrder, Shader* in_pShader)
    : Component(in_pOwner, 100)
{
    GraphicData graphicData(in_pShader, nullptr);
    this->_Init(in_drawOrder, graphicData);
}

/// <summary>
/// Initializes a new instance of the <see cref="SpriteComponent" /> class.
/// </summary>
/// <param name="in_pOwner">The in p owner.</param>
/// <param name="in_drawOrder">The in draw order.</param>
/// <param name="in_rGraphicData">The in r graphic data.</param>
SpriteComponent::SpriteComponent(Actor* in_pOwner, const int in_drawOrder,
                                 GraphicData& in_rGraphicData)
    : Component(in_pOwner, 100)
{
    this->_Init(in_drawOrder, in_rGraphicData);
}

/// <summary>
/// Finalizes an instance of the <see cref="SpriteComponent"/> class.
/// </summary>
SpriteComponent::~SpriteComponent()
{
    auto pRenderer = this->pOwner->GetRenderer();
    pRenderer->RemoveSprite(this);
}

/// <summary>
/// Updates the specified in delta time.
/// </summary>
/// <param name="in_deltaTime">The in delta time.</param>
/// <returns></returns>
auto SpriteComponent::Update(const float /*in_deltaTime*/) -> void
{
}

/// <summary>
/// Sets the texture.
/// </summary>
/// <param name="in_pTexture">The in p texture.</param>
/// <returns></returns>
auto SpriteComponent::SetTexture(Texture* in_pTexture) -> void
{
    SDL_assert((in_pTexture != nullptr));

    this->grapchiData.pTexture = in_pTexture;
}

/// <summary>
/// Draws the specified in p renderer.
/// </summary>
/// <param name="in_pRenderer">The in p renderer.</param>
/// <returns></returns>
auto SpriteComponent::Draw() -> void
{
    s_pVertexArray->SetActive();

    auto pTexture = this->grapchiData.pTexture;
    auto world    = Math::Matrix4::CreateScale(static_cast<float>(pTexture->GetWidth()),
                                               static_cast<float>(pTexture->GetHeight()), 1.0f);
    {
        auto orgWorld = this->pOwner->GetWorldTransform();
        world.Mul(orgWorld);
    }

    auto pShader = this->grapchiData.pShader;
    pShader->SetMatrixUniform("uWorldTransform", &world);
    pShader->SetTextureUniform("uTexture", 0);

    pTexture->SetActive();

    // 描画
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

/// <summary>
/// Initializes the specified in draw order.
/// </summary>
/// <param name="in_drawOrder">The in draw order.</param>
/// <param name="in_rGraphicData">The in r graphic data.</param>
/// <returns></returns>
auto SpriteComponent::_Init(const int in_drawOrder, GraphicData& in_rGraphicData) -> void
{
    this->_Clear();
    this->grapchiData = in_rGraphicData;

    this->drawOrder = in_drawOrder;
    auto pRenderer  = this->pOwner->GetRenderer();
    pRenderer->AddSprite(this);

    // 頂点配列作成
    {
        static float points[] = {
            // 座標軸がデカルト座標系に準拠
            // x    y     z    nx   ny   nz   u    v
            -0.5f, 0.5f,  0.f, 0.f, 0.f, 0.f, 0.f, 0.f,  // top left
            0.5f,  0.5f,  0.f, 0.f, 0.f, 0.f, 1.f, 0.f,  // top right
            0.5f,  -0.5f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  // bottom right
            -0.5f, -0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f   // bottom left
        };

        static unsigned int indices[] = {0, 1, 2, 2, 3, 0};

        if (s_pVertexArray == nullptr)
        {
            s_pVertexArray = new VertexArray(points, 4, indices, 6);
        }
    }
}