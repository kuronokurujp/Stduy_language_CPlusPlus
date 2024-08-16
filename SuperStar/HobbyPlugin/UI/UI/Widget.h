﻿#pragma once

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

    private:
        Core::Common::Handle _inputHandle;
    };
}  // namespace UI
