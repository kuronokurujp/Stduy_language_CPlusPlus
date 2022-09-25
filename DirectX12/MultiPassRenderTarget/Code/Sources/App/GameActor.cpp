#include "App/GameActor.h"

#include "Component/SkeletalMeshComponent.h"

namespace App
{
    void GameActor::Render()
    {
        // メッシュコンポーネントがあれば描画実行
        // メッシュコンポーネントは一つのみしか認めていない
        auto mesh = this->GetComponent<Component::SkeltalMeshCameraComponent*>();
        if (mesh == nullptr)
            return;

        mesh->Render();
    }
}
