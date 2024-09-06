#pragma once

#include <functional>

#include "Engine/MiniEngine.h"
#include "LevelBaseComponent.h"

namespace Level
{
    /// <summary>
    /// イベント受け取るためのハンドラー
    /// </summary>
    using LevelUserInputEventHandler = std::function<void(const Char*)>;

    class LevelUserInputMessage final
    {
        HE_CLASS_COPY_NG(LevelUserInputMessage);
        HE_CLASS_MOVE_NG(LevelUserInputMessage);

    public:
        LevelUserInputMessage() {}
        LevelUserInputMessage(LevelUserInputEventHandler in_func) : _eventHandler(in_func) {}

        void Message(const Char* in_szMsg) { this->_eventHandler(in_szMsg); }

    private:
        LevelUserInputEventHandler _eventHandler;
    };

    /// <summary>
    /// UIの入力を受け取るレベル専用コンポーネント
    /// </summary>
    class LevelUserInputReceiveComponent final : public LevelBaseComponent
    {
        HE_CLASS_COPY_NG(LevelUserInputReceiveComponent);
        HE_CLASS_MOVE_NG(LevelUserInputReceiveComponent);
        HE_GENERATED_CLASS_BODY_HEADER(LevelUserInputReceiveComponent, LevelBaseComponent);

    public:
        LevelUserInputReceiveComponent() : LevelBaseComponent() {}

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void VSetup(const Bool in_bReleaseMem = TRUE) override;

        void Message(const Char* in_szMsg);

        void SetReceiver(std::unique_ptr<LevelUserInputMessage, DeleterFreeMemory> in_spReciver)
        {
            this->_spEventReceiver = std::move(in_spReciver);
        }

    private:
        std::unique_ptr<LevelUserInputMessage, DeleterFreeMemory> _spEventReceiver;
    };
}  // namespace Level
