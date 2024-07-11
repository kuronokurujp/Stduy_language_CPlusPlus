#pragma once

#include <functional>

#include "HobbyPlugin/Actor/Component/Component.h"
#include "MiniEngine.h"

namespace Level
{
    /// <summary>
    /// イベント受け取るためのハンドラー
    /// </summary>
    using LevelUserInputEventHandler = std::function<void(const Char*)>;

    /// <summary>
    /// </summary>
    class LevelUserInputMessage
    {
        E_CLASS_COPY_CONSTRUCT_NG(LevelUserInputMessage);

    public:
        LevelUserInputMessage(LevelUserInputEventHandler in_func) : _eventHandler(in_func) {}

        void Message(const Char* in_pMsg) { this->_eventHandler(in_pMsg); }

    private:
        LevelUserInputEventHandler _eventHandler;
    };

    /// <summary>
    /// UIの入力を受け取るレベル専用コンポーネント
    /// </summary>
    class LevelUserInputReceiveComponent : public Actor::Component
    {
        E_CLASS_COPY_CONSTRUCT_NG(LevelUserInputReceiveComponent);
        GENERATED_CLASS_BODY_HEADER(LevelUserInputReceiveComponent, Actor::Component);

    public:
        LevelUserInputReceiveComponent() : Actor::Component() {}

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void Setup(const Bool in_bReleaseMem = TRUE) override;

        void Message(const Char* in_pMsg);

        void SetReceiver(std::unique_ptr<LevelUserInputMessage> in_reciver)
        {
            this->_eventReceiver = std::move(in_reciver);
        }

    private:
        std::unique_ptr<LevelUserInputMessage> _eventReceiver;
    };
}  // namespace Level
