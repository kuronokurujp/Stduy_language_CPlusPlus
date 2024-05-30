#pragma once

#include "Core/Core.h"
#include "Core/Common/FixArray.h"

#include "Module/Module.h"

// 前方宣言
namespace Core
{
    namespace Memory
    {
        class Manager;
    }
}

namespace Platform
{
    // 前方宣言
    class InputSystemInterface;

    /// <summary>
    /// プラットフォームの時間インターフェイス
    /// </summary>
    class TimeSystemInterface
    {
    public:
        virtual const Uint32  NowMSec() = 0;
        virtual void SleepMSec(const Uint32 in_ms) = 0;
    };

    /// <summary>
    /// プラットフォームの追加モジュール
    /// ゲームは一つのプラットフォームしか起動しない
    /// この継承したクラスのインスタンスは必ず一つになっている
    /// エンジンが起動時には必ずこのプラットフォームモジュールは設定しないとエラーになる
    /// </summary>
    class PlatformModule : public Module::ModuleBase<PlatformModule>
    {
    public:
        /// <summary>
        /// TODO: モジュール初期化
        /// </summary>
        /// <returns></returns>
        virtual const Bool Init() override { return FALSE; }

        /// <summary>
        /// TODO: モジュール終了
        /// </summary>
        /// <returns></returns>
        virtual const Bool End() override { return FALSE; }

        virtual const Bool CreateMainWindow() { return FALSE; }
        virtual const Bool ReleaseAllWindows() { return FALSE; }

        /// <summary>
        /// このモジュールだけ更新前処理と後処理が必要なので用意している
        /// エンジン側で呼び出している
        /// </summary>
        /// <param name="in_deltaTime"></param>
        /// <returns></returns>
        virtual const Bool BeforUpdate(const Float32 in_deltaTime) { return FALSE; }
        virtual const Bool AfterUpdate(const Float32 in_deltaTime) { return FALSE; }

        virtual const Bool Update(const Float32 in_deltaTime) override { return FALSE; }

        virtual TimeSystemInterface* Time() = 0;
        // 入力システムを取得
        virtual InputSystemInterface* Input() = 0;

        // 描画
        virtual void BeginRender(void* in_pCmdArray) = 0;
        virtual void Redner(void* in_pCmdArray) = 0;
        virtual void EndRender(void* in_pCmdArray) = 0;
    };
}
