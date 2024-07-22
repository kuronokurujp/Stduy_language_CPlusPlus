#include "UIText.h"

// 依存するモジュール一覧
#include "ActorModule.h"
#include "LocalizationModule.h"
#include "RenderModule.h"

namespace UI
{

    void UITextComponent::Update(const Float32 in_fDeltaTime)
    {
        // TODO: 描画コマンドを追加
        Core::Math::Rect2 rect;
        this->TransformLocalToWorldRect2D(&rect, this->_rect);

        // TODO: ローカライズテキストならローカライズテキストを取得
        if (0 < this->_szLocGroup.Length())
        {
            // TODO: 言語切り替えが必要
            Core::Common::FixString1024 str(
                ModuleLocalization()->Text(Core::Common::FixString16(HE_STR_TEXT("JP")),
                                           this->_szLocGroup, this->_szText));
            Render::Cmd2DText(rect.Pos(), str, {this->_color});
        }
        else
        {
            Render::Cmd2DText(rect.Pos(), this->_szText, {this->_color});
        }
    }
}  // namespace UI
