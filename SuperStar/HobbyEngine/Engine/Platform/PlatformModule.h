#pragma once

#include "Engine/Core.h"
#include "Engine/Module/Module.h"

// プラットフォームのインターフェイス
#include "PlatformFile.h"
#include "PlatformInput.h"
#include "PlatformScreen.h"
#include "PlatformSystem.h"
#include "PlatformTime.h"

namespace Platform
{
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
        virtual const Bool IsQuit() = 0;

        /// <summary>
        /// 時間関連の処理
        /// </summary>
        virtual TimeInterface* Time() = 0;

        /// <summary>
        /// 入力関連の処理
        /// </summary>
        virtual InputInterface* Input() = 0;

        /// <summary>
        /// ファイル関連の処理
        /// </summary>
        virtual FileInterface* File() = 0;

        /// <summary>
        /// スクリーン関連の処理
        /// </summary>
        virtual ScreenInterface* Screen() = 0;

        /// <summary>
        /// システム関連の処理
        /// </summary>
        virtual SystemInterface* System() = 0;

    protected:
        /// <summary>
        /// モジュール開始
        /// </summary>
        virtual const Bool _Start() override
        {
            HE_ASSERT(FALSE);
            return FALSE;
        }

        /// <summary>
        /// 前更新
        /// </summary>
        /// <param name="in_fDeltaTime"></param>
        /// <returns></returns>
        virtual const Bool _BeforeUpdate(const Float32 in_fDeltaTime) override = 0;

        /// <summary>
        /// 更新
        /// </summary>
        /// <param name="in_fDeltaTime"></param>
        /// <returns></returns>
        virtual const Bool _Update(const Float32 in_fDeltaTime) override = 0;

        /// <summary>
        /// 後更新
        /// </summary>
        /// <param name="in_fDeltaTime"></param>
        /// <returns></returns>
        virtual const Bool _LateUpdate(const Float32 in_fDeltaTime) override = 0;
    };
}  // namespace Platform
