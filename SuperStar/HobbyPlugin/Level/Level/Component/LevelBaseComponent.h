#pragma once

#include "Actor/Component/Component.h"
#include "Level/LevelNode.h"

// エンジン最小インクルード
#include "Engine/MiniEngine.h"

namespace Level
{
    /// <summary>
    /// レベルに設定できる基本コンポーネント
    /// レベル用コンポーネントは必ずこれを継承しないといけない
    /// </summary>
    class LevelBaseComponent : public Actor::Component
    {
        HE_CLASS_COPY_NG(LevelBaseComponent);
        HE_CLASS_MOVE_NG(LevelBaseComponent);
        HE_GENERATED_CLASS_BODY_HEADER(LevelBaseComponent, Actor::Component);

    public:
        LevelBaseComponent() : Actor::Component() {}
        virtual ~LevelBaseComponent() {}

        /// <summary>
        /// レベルにアクターを追加
        /// </summary>
        template <class T>
        Core::Common::Handle AddActor()
        {
            auto pNode = reinterpret_cast<Node*>(this->_pOwner);
            HE_ASSERT(pNode);
            return pNode->AddActor<T>();
        }

        // レベルに追加されたアクターを削除
        void RemoveActor(Core::Common::Handle*);

        /// <summary>
        /// レベルのアクターを取得
        /// </summary>
        template <class T>
        T* GetActor(const Core::Common::Handle& in_rHandle)
        {
            auto pNode = reinterpret_cast<Node*>(this->_pOwner);
            HE_ASSERT(pNode);

            return pNode->GetActor<T>(in_rHandle);
        }
    };

}  // namespace Level
