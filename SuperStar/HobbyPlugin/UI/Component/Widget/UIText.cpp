#include "UIText.h"

// 依存するモジュール一覧
#include "HobbyPlugin/Actor/Actor.h"
#include "HobbyPlugin/Localization/LocalizationModule.h"
#include "HobbyPlugin/Render/Command/Command.h"

namespace UI
{

    void UITextComponent::Update(const Float32 in_deltaTime)
    {
        // TODO: 描画コマンドを追加
        Core::Math::Rect2 rect;
        this->TransformLocalToWorldRect2D(&rect, this->_rect);

        // TODO: ローカライズテキストならローカライズテキストを取得
        if (0 < this->_locGroup.Length())
        {
            // TODO: 言語切り替えが必要
            Core::Common::FixString1024 str(
                ModuleLocalization()->Text(E_STR_TEXT("JP"), this->_locGroup, this->_str.Str()));
            Render::Cmd2DText(rect.Pos(), str, {this->_color});
        }
        else
        {
            Render::Cmd2DText(rect.Pos(), this->_str, {this->_color});
        }
    }
}  // namespace UI
