#pragma once

#include "MiniEngine.h"

// 外部モジュール
#include "HobbyPlugin/Actor/Actor.h"

namespace UI
{
    /// <summary>
    /// UIの基本Widget
    /// </summary>
    class Widget : public Actor::Object
    {
    public:
        /// <summary>
        /// 開始
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        virtual const Bool Begin() override;

        /// <summary>
        /// 終了
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        virtual const Bool End() override;

        /// <summary>
        /// 子アクターが追加された時に呼ばれるイベント
        /// </summary>
        /// <param name="in_pChildActor"></param>
        virtual void OnAddChildActor(Actor::Object* in_pChildActor) final override;

        /// <summary>
        /// 子アクター外す時に呼ばれるイベント
        /// </summary>
        /// <param name="in_pChildActor"></param>
        virtual void OnRemoveChildActor(Actor::Object* in_pChildActor) final override;

    protected:
        virtual void _ProcessInput(const Float32, Platform::InputSystemInterface*) final override;

    private:
        Core::Common::Handle _inputHandle;
    };
}  // namespace UI
