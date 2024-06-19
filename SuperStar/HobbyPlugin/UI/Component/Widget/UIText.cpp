#include "UIText.h"

#include "Core/Math/Vector3.h"

// 依存するモジュール一覧
#include "HobbyPlugin/Render/Command/Command.h"
#include "HobbyPlugin/Actor/Actor.h"

namespace UI
{
    GENERATED_CLASS_BODY(UITextComponent, UIWidgetComponent);

    void UITextComponent::Update(const Float32 in_deltaTime)
    {
        // TODO: 描画コマンドを追加
        Core::Math::Vector2 textPos;
        this->TransformLocalToWorldPos2D(&textPos, this->_rect.Pos());

        Render::Cmd2DText(textPos, this->_str, {this->_color});
    }
}
