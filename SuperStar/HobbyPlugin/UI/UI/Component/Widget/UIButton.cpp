#include "UIButton.h"

#include "Engine/Math/Vector3.h"

// 依存するモジュール一覧
#include "ActorModule.h"
#include "RenderModule.h"

namespace UI
{
    void UIButtonComponent::Setup(const Bool in_bAutoDelete)
    {
        UIWidgetComponent::Setup(in_bAutoDelete);

        this->_Clear();
    }

    /// <summary>
    /// ボタンコンポーネントが終了
    /// </summary>
    /// <returns></returns>
    const Bool UIButtonComponent::End()
    {
        const Bool bRet = UIWidgetComponent::End();

        // プッシュハンドルを解放
        this->_pushHandler.release();

        return bRet;
    }

    /// <summary>
    /// コンポーネントの更新
    /// 必ず処理を記述
    /// </summary>
    void UIButtonComponent::Update(const Float32 in_fDeltaTime)
    {
        Core::Math::Rect2 srcRect(0.0f, 0.0f, this->_fWidth, this->_fHeight, this->_eAnchor);
        Core::Math::Rect2 rect;
        this->TransformLocalToWorldRect2D(&rect, srcRect);

        // 描画コマンドを追加
        Render::Cmd2DRect(rect, {this->_color});
    }

    void UIButtonComponent::OnTouch(const EnhancedInput::InputData::Item::Touch& in_rTouch)
    {
        Core::Math::Rect2 rect;
        Core::Math::Rect2 orgRect(0.0f, 0.0f, this->_fWidth, this->_fHeight, this->_eAnchor);
        this->TransformLocalToWorldRect2D(&rect, orgRect);

        if (rect.InSidePoint(Core::Math::Vector2(in_rTouch.fX, in_rTouch.fY)))
        {
            this->_pushHandler->OnPush();
        }
    }

}  // namespace UI
