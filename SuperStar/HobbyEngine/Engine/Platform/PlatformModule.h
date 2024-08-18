#pragma once

#include "Engine/Common/CustomArray.h"
#include "Engine/Core.h"
#include "Engine/Module/Module.h"

namespace Platform
{
    // 前方宣言
    class InputSystemInterface;
    class FileSystemInterface;

    /// <summary>
    /// プラットフォームの時間インターフェイス
    /// </summary>
    class TimeSystemInterface
    {
    public:
        virtual const Uint32 NowMSec()             = 0;
        virtual void SleepMSec(const Uint32 in_ms) = 0;
    };

    /// <summary>
    /// プラットフォームの追加モジュール
    /// ゲームは一つのプラットフォームしか起動しない
    /// この継承したクラスのインスタンスは必ず一つになっている
    /// エンジンが起動時には必ずこのプラットフォームモジュールは設定しないとエラーになる
    /// </summary>
    class PlatformModule : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(PlatformModule);

    public:
        // プラットフォームモジュールはOS固有なのでモジュールレイヤーはアプリに
        PlatformModule() : ModuleBase(ModuleName(), Module::eLayer_App) {}

        virtual const Bool CreateMainWindow() { return FALSE; }
        virtual const Bool ReleaseAllWindows() { return FALSE; }

        /// <summary>
        /// やめる状態になっているか
        /// </summary>
        /// <returns></returns>
        virtual const Bool IsQuit() = 0;

        /// <summary>
        /// 時間システム
        /// </summary>
        /// <returns></returns>
        virtual TimeSystemInterface* Time() = 0;

        /// <summary>
        /// 入力システム
        /// </summary>
        /// <returns></returns>
        virtual InputSystemInterface* Input() = 0;

        /// <summary>
        /// ファイルシステム
        /// </summary>
        /// <returns></returns>
        virtual FileSystemInterface* File() = 0;

    protected:
        /// <summary>
        /// モジュール開始
        /// </summary>
        /// <returns></returns>
        virtual const Bool _Start() override
        {
            HE_ASSERT(FALSE);
            return FALSE;
        }

        /// <summary>
        /// モジュール前更新
        /// </summary>
        /// <param name="in_fDeltaTime"></param>
        /// <returns></returns>
        virtual const Bool _BeforeUpdate(const Float32 in_fDeltaTime) override
        {
            HE_ASSERT(FALSE);
            return FALSE;
        }

        virtual const Bool _Update(const Float32 in_fDeltaTime) override
        {
            HE_ASSERT(FALSE);
            return FALSE;
        }

        virtual const Bool _LateUpdate(const Float32 in_fDeltaTime) override
        {
            HE_ASSERT(FALSE);
            return FALSE;
        }
    };
}  // namespace Platform
