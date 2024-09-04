#pragma once

#include "Engine/MiniEngine.h"

// 外部モジュール
#include "ActorModule.h"

namespace UI
{
    /// <summary>
    /// UIの基本Widget
    /// </summary>
    class Widget : public Actor::Object
    {
        HE_CLASS_COPY_NG(Widget);
        HE_CLASS_MOVE_NG(Widget);

    public:
        Widget() : Actor::Object() {}
        virtual ~Widget() = default;

        /// <summary>
        /// 開始
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        virtual const Bool VBegin() override;

        /// <summary>
        /// 終了
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        virtual const Bool VEnd() override;
    };
}  // namespace UI
