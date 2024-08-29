#pragma once

#include "Actor/Component/InputComponent.h"
#include "Engine/Common/Handle.h"
#include "Engine/MiniEngine.h"

namespace Actor
{
    class ActorManager;

    /// <summary>
    /// アクター管理を拡張するデコレーターインターフェイス
    /// </summary>
    class ActorManagerDecoraterlnterface
    {
    public:
        virtual const Bool VStart(ActorManager*) = 0;

        virtual void VLateUpdate(const Float32 in_fDt, ActorManager*) = 0;

        /// <summary>
        /// アクターのコンポーネントの登録・解除イベント
        /// </summary>
        virtual void VOnActorRegistComponent(Component*)   = 0;
        virtual void VOnActorUnRegistComponent(Component*) = 0;
    };

}  // namespace Actor
