#pragma once

#include "UIWidget.h"

namespace UI
{
    /// <summary>
    /// UIのレイヤー
    /// このレイヤー以下でないとボタンやテキストのオブジェクト配置できない
    /// </summary>
    class UILayerComponent final : public UIWidgetComponent
    {
        HE_CLASS_COPY_NG(UILayerComponent);
        HE_GENERATED_CLASS_BODY_HEADER(UILayerComponent, UIWidgetComponent);

    public:
        UILayerComponent() : UIWidgetComponent() {}
    };
}  // namespace UI
