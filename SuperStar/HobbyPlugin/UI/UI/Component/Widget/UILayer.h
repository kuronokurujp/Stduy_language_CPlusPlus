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
        HE_CLASS_COPY_NG(UILayerComponent);
        GENERATED_CLASS_BODY_HEADER(UILayerComponent, UIWidgetComponent);

    public:
        UILayerComponent() : UIWidgetComponent() {}
        /// <summary>
        /// コンポーネントの更新
        /// 必ず処理を記述
        /// </summary>
        void Update(const Float32 in_fDeltaTime) override;
    };
}  // namespace UI
