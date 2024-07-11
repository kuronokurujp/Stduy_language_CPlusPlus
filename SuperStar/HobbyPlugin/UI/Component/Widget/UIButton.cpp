#include "UIButton.h"

#include "Core/Math/Vector3.h"
#include "HobbyPlugin/Actor/Actor.h"

// 依存するモジュール一覧
#include "HobbyPlugin/Render/Command/Command.h"

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
    /// <param name="in_deltaTime">The in delta time.</param>
    void UIButtonComponent::Update(const Float32 in_deltaTime)
    {
        Core::Math::Rect2 srcRect(0.0f, 0.0f, this->_w, this->_h, Core::Math::Rect2::EPivot_Left);
        Core::Math::Rect2 rect;
        this->TransformLocalToWorldRect2D(&rect, srcRect);

        // TODO: 描画コマンドを追加
        Render::Cmd2DRect(rect, {this->_color});
    }

    void UIButtonComponent::OnTouch(const Platform::TouchInput& in_rTouch)
    {
        if (in_rTouch.GetTouchState(Platform::EInputMouseType_All) ==
            Platform::EInputState::EInputState_PRESSED)
        {
            Core::Math::Rect2 rect;
            Core::Math::Rect2 orgRect(0.0f, 0.0f, this->_w, this->_w,
                                      Core::Math::Rect2::EPivot_Left);
            this->TransformLocalToWorldRect2D(&rect, orgRect);

            // TODO: ボタンにタッチしたらコールバックを呼ぶ
            if (in_rTouch.IsTouchInRect(rect))
            {
                E_LOG_LINE(E_STR_TEXT("Button Pressed"));
                this->_pushHandler->OnPush();
            }
        }
    }
}  // namespace UI
