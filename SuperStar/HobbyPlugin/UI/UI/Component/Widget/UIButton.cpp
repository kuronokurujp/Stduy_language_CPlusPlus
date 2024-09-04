#include "UIButton.h"

#include "Engine/Math/Vector3.h"

// 依存するモジュール一覧
#include "ActorModule.h"
#include "RenderModule.h"

namespace UI
{
    void UIButtonComponent::VSetup(const Bool in_bAutoDelete)
    {
        UIWidgetComponent::VSetup(in_bAutoDelete);

        this->_Clear();
    }

    /// <summary>
    /// ボタンコンポーネントが終了
    /// </summary>
    /// <returns></returns>
    const Bool UIButtonComponent::VEnd()
    {
        const Bool bRet = UIWidgetComponent::VEnd();

        // プッシュハンドルを解放
        this->_pushHandler.release();

        return bRet;
    }

    /// <summary>
    /// コンポーネントの更新
    /// 必ず処理を記述
    /// </summary>
    void UIButtonComponent::VUpdate(const Float32 in_fDeltaTime)
    {
        Core::Math::Rect2 srcRect(0.0f, 0.0f, this->_fWidth, this->_fHeight, this->_eAnchor);
        Core::Math::Rect2 rect;

        auto pTrans = this->GetTransformComponent();
        HE_ASSERT(pTrans);

        pTrans->TransformLocalToWorldRect2D(&rect, srcRect);

        // 描画コマンドを追加
        Render::CreateCmd2DRectDraw(this->_viewHandle, rect, {this->_color});
    }

    void UIButtonComponent::VOnTouch(const EnhancedInput::InputData::Item::Touch& in_rTouch)
    {
        Core::Math::Rect2 rect;
        Core::Math::Rect2 orgRect(0.0f, 0.0f, this->_fWidth, this->_fHeight, this->_eAnchor);

        auto pTrans = this->GetTransformComponent();
        HE_ASSERT(pTrans);

        pTrans->TransformLocalToWorldRect2D(&rect, orgRect);

        if (rect.InSidePoint(Core::Math::Vector2(in_rTouch.fX, in_rTouch.fY)))
        {
            this->_pushHandler->OnPush();
        }
    }

}  // namespace UI
