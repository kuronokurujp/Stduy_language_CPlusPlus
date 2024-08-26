#include "UIText.h"

// 依存するモジュール一覧
#include "ActorModule.h"
#include "LocalizationModule.h"
#include "RenderModule.h"

namespace UI
{
    void UITextComponent::Update(const Float32 in_fDeltaTime)
    {
        // ローカライズテキストならローカライズテキストを取得
        if (0 < this->_szLocGroup.Length())
        {
            // TODO: 言語切り替えが必要
            auto pLocalModule = Module::ModuleManager::I().Get<Localization::LocalizationModule>();
            HE_ASSERT(pLocalModule);

            this->_szDrawText = pLocalModule->Text(Core::Common::FixString16(HE_STR_TEXT("JP")),
                                                   this->_szLocGroup, this->_szText);
        }
        else
        {
            this->_szDrawText = this->_szText;
        }

        Core::Math::Vector2 pos;
        this->TransformLocalToWorldPos2D(&pos, this->_rect.Pos());

        Render::CreateCmd2DTextDraw(this->_viewHandle, pos, this->_szDrawText, {this->_color},
                                    this->_eAnchor);
    }
}  // namespace UI
