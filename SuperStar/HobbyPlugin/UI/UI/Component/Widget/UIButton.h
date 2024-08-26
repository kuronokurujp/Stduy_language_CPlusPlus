#pragma once

#include <functional>
#include <memory>

#include "Engine/Math/Rect2.h"
#include "Engine/MiniEngine.h"
#include "UIWidget.h"

namespace UI
{
    /// <summary>
    /// ボタンのプッシュ通知ハンドラー
    /// </summary>
    class UIButtonMessageHandler
    {
        HE_CLASS_COPY_NG(UIButtonMessageHandler);

    public:
        UIButtonMessageHandler()          = default;
        virtual ~UIButtonMessageHandler() = default;

        void OnPush() { this->_OnPushInternal(); }

    protected:
        virtual void _OnPushInternal() = 0;
    };

    using UIButtonMessageHandlerImpOnPush = std::function<void(Core::Common::StringBase&)>;

    /// <summary>
    /// ボタンのプッシュ通知
    /// デフォルト版
    /// </summary>
    class UIButtonMessageHandlerDefault : public UIButtonMessageHandler
    {
        HE_CLASS_COPY_NG(UIButtonMessageHandlerDefault);

    public:
        UIButtonMessageHandlerDefault() = default;
        UIButtonMessageHandlerDefault(const Char* in_szMsg, UIButtonMessageHandlerImpOnPush in_func)
            : _onPush(in_func), _msg(in_szMsg)
        {
        }

    protected:
        void _OnPushInternal() override final { this->_onPush(this->_msg); }

    private:
        UIButtonMessageHandlerImpOnPush _onPush;
        Core::Common::FixString128 _msg;
    };

    /// <summary>
    /// UIに表示するボタンコンポーネント
    /// タッチイベントはコンポーネントUIInputTerminalTouchで呼び出している
    /// </summary>
    class UIButtonComponent : public UIWidgetComponent
    {
        HE_CLASS_COPY_NG(UIButtonComponent);
        GENERATED_CLASS_BODY_HEADER(UIButtonComponent, UIWidgetComponent);

    public:
        UIButtonComponent() : UIWidgetComponent() { this->_Clear(); }

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void Setup(const Bool in_bAutoDelete = TRUE) override;

        /// <summary>
        /// コンポーネントの終了
        /// </summary>
        virtual const Bool End() override;

        /// <summary>
        /// コンポーネントの更新
        /// 必ず処理を記述
        /// </summary>
        void Update(const Float32 in_fDeltaTime) override;

        /// <summary>
        /// プッシュ通知のハンドラーを設定
        /// ユニークポインタで所有権を移譲している
        /// </summary>
        void SetPushHandler(std::unique_ptr<UIButtonMessageHandler, DeleterFreeMemory> in_spHandler)
        {
            this->_pushHandler = std::move(in_spHandler);
        }

        void SetWidth(const Float32 in_fW) { this->_fWidth = in_fW; }
        void SetHeight(const Float32 in_fH) { this->_fHeight = in_fH; }
        void SetAnchor(const Core::Math::Rect2::EAnchor in_eAnchor) { this->_eAnchor = in_eAnchor; }

        /// <summary>
        /// タッチイベント
        /// </summary>
        void OnTouch(const EnhancedInput::InputData::Item::Touch& in_rTouch) override;

    private:
        void _Clear()
        {
            UIWidgetComponent::_Clear();

            this->_pushHandler.release();
            this->_fWidth  = 0.0f;
            this->_fHeight = 0.0f;
            this->_eAnchor = Core::Math::Rect2::EAnchor_Left;
        }

    private:
        std::unique_ptr<UIButtonMessageHandler, DeleterFreeMemory> _pushHandler;
        Float32 _fWidth                     = 0.0f;
        Float32 _fHeight                    = 0.0f;
        Core::Math::Rect2::EAnchor _eAnchor = Core::Math::Rect2::EAnchor_Left;
    };
}  // namespace UI
