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
        GENERATED_CLASS_BODY_HEADER(UILayerComponent, UIWidgetComponent);

    public:
        /// <summary>
        /// コンポーネントの更新
        /// 必ず処理を記述
        /// </summary>
        void Update(const Float32 in_fDeltaTime) override;
    };
}  // namespace UI
