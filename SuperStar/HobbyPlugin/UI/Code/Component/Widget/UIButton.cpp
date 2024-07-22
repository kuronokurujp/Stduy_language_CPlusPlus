#include "UIButton.h"

#include "Math/Vector3.h"

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
        Core::Math::Rect2 srcRect(0.0f, 0.0f, this->_fWidth, this->_fHeight,
                                  Core::Math::Rect2::EPivot_Left);
        Core::Math::Rect2 rect;
        this->TransformLocalToWorldRect2D(&rect, srcRect);

        // 描画コマンドを追加
        Render::Cmd2DRect(rect, {this->_color});
    }

    void UIButtonComponent::OnTouch(const Platform::TouchInput& in_rTouch)
    {
        if (in_rTouch.GetTouchState(Platform::EInputMouseType_All) ==
            Platform::EInputState::EInputState_PRESSED)
        {
            Core::Math::Rect2 rect;
            Core::Math::Rect2 orgRect(0.0f, 0.0f, this->_fWidth, this->_fWidth,
                                      Core::Math::Rect2::EPivot_Left);
            this->TransformLocalToWorldRect2D(&rect, orgRect);

            // ボタンにタッチしたらコールバックを呼ぶ
            if (in_rTouch.IsTouchInRect(rect))
            {
                HE_LOG_LINE(HE_STR_TEXT("Button Pressed"));
                this->_pushHandler->OnPush();
            }
        }
    }
}  // namespace UI
