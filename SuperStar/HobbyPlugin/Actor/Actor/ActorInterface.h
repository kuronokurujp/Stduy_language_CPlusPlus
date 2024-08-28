#pragma once

#include "Actor/Component/InputComponent.h"
#include "Engine/Common/Handle.h"
#include "Engine/MiniEngine.h"

namespace Actor
{
    /// <summary>
    /// アクター管理を拡張するデコレーターインターフェイス
    /// </summary>
    class ActorManagerDecoraterlnterface
    {
    public:
        /// <summary>
        /// アクターのコンポーネントの登録・解除イベント
        /// </summary>
        virtual void VOnActorRegistComponent(Component*)   = 0;
        virtual void VOnActorUnRegistComponent(Component*) = 0;
    };

}  // namespace Actor
