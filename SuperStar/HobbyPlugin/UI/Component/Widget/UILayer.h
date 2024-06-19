#pragma once

#include "UIWidget.h"

namespace UI
{
    /// <summary>
    /// UIのレイヤー
    /// このレイヤー以下でないとボタンやテキストのオブジェクト配置できない
    /// </summary>
    class UILayerComponent : public UIWidgetComponent
    {
        GENERATED_CLASS_BODY_HEADER();

    public:
        /// <summary>
        /// コンポーネントの更新
        /// 必ず処理を記述
        /// </summary>
        /// <param name="in_deltaTime">The in delta time.</param>
        void Update(const Float32 in_deltaTime) final override;
    };
}
