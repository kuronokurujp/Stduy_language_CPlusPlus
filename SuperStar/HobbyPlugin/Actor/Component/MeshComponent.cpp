#include "Component/meshComponent.h"

#include "3D/mesh.h"
#include "3D/shader.h"
#include "3D/vertexArray.h"
#include "Actor/actor.h"
#include "renderer.h"
#include "texture.h"

MeshComponent::MeshComponent(Actor* in_pOwner) : Component(in_pOwner, 0)
{
    this->pOwner->GetRenderer()->AddMesh(this);
}

MeshComponent::~MeshComponent()
{
    this->pOwner->GetRenderer()->RemoveMesh(this);
}

void MeshComponent::Update(const float /*in_deltaTime*/)
{
}

void MeshComponent::Draw(Shader* in_pShader)
{
    if (in_pShader == nullptr)
    {
        return;
    }

    if (this->pMesh == nullptr)
    {
        return;
    }

    in_pShader->SetMatrixUniform("uWorldTransform", &this->pOwner->GetWorldTransform());
    // 光沢値を設定
    in_pShader->SetFloatUniform("uSpecPower", this->pMesh->GetSpecPower());

    auto t = this->pMesh->GetTexture(this->textureIndex);
    if (t != nullptr)
    {
        t->SetActive();
    }

    auto va = this->pMesh->GetVertexArray();
    va->SetActive();

    glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
}