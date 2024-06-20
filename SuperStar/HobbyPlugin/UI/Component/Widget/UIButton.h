#pragma once

#include <functional>
#include <memory>

#include "Core/Math/Rect2.h"
#include "MiniEngine.h"
#include "UIWidget.h"

namespace UI
{
    /// <summary>
    /// ボタンのプッシュ通知ハンドラー
    /// </summary>
    class UIButtonMessageHandler
    {
    public:
        UIButtonMessageHandler()          = default;
        virtual ~UIButtonMessageHandler() = default;

        void OnPush() { this->_OnPushInternal(); }

    protected:
        virtual void _OnPushInternal() = 0;
    };

    using UIButtonMessageHandlerImpOnPush = std::function<void(Core::Common::FixString128&)>;

    /// <summary>
    /// ボタンのプッシュ通知
    /// デフォルト版
    /// </summary>
    class UIButtonMessageHandlerDefault : public UIButtonMessageHandler
    {
    public:
        UIButtonMessageHandlerDefault(Core::Common::FixString128 in_msg,
                                      UIButtonMessageHandlerImpOnPush in_func)
            : _onPush(in_func), _msg(in_msg)
        {
        }

        virtual ~UIButtonMessageHandlerDefault() = default;

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
        E_CLASS_COPY_CONSTRUCT_NG(UIButtonComponent);
        GENERATED_CLASS_BODY_HEADER();

    public:
        UIButtonComponent() : UIWidgetComponent() { this->_Clear(); }

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void Init(const Bool in_bAutoDelete = TRUE) override;

        /// <summary>
        /// コンポーネントの終了
        /// </summary>
        virtual const Bool End() override;

        /// <summary>
        /// コンポーネントの更新
        /// 必ず処理を記述
        /// </summary>
        /// <param name="in_deltaTime">The in delta time.</param>
        void Update(const Float32 in_deltaTime) final override;

        /// <summary>
        /// プッシュ通知のハンドラーを設定
        /// ユニークポインタで所有権を移譲している
        /// </summary>
        /// <param name="in_hanlder"></param>
        void SetPushHandler(std::unique_ptr<UIButtonMessageHandler> in_hanlder)
        {
            this->_pushHandler = std::move(in_hanlder);
        }

        /// <summary>
        /// タッチイベント
        /// </summary>
        /// <param name="in_rTouch"></param>
        virtual void OnTouch(const Platform::TouchInput& in_rTouch) override;

    private:
        void _Clear() { this->_pushHandler.release(); }

    private:
        std::unique_ptr<UIButtonMessageHandler> _pushHandler;
    };
}  // namespace UI
