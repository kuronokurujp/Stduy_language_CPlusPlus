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
        PlatformModule() : ModuleBase(ModuleName(), Module::ELayer_App) {}
        virtual ~PlatformModule() = default;

        virtual const Bool VCreateMainWindow() { return FALSE; }
        virtual const Bool VReleaseAllWindows() { return FALSE; }

        /// <summary>
        /// やめる状態になっているか
        /// </summary>
        virtual const Bool VIsQuit() = 0;

        /// <summary>
        /// 時間関連の処理
        /// </summary>
        virtual TimeInterface* VTime() = 0;

        /// <summary>
        /// 入力関連の処理
        /// </summary>
        virtual InputInterface* VInput() = 0;

        /// <summary>
        /// ファイル関連の処理
        /// </summary>
        virtual FileInterface* VFile() = 0;

        /// <summary>
        /// スクリーン関連の処理
        /// </summary>
        virtual ScreenInterface* VScreen() = 0;

        /// <summary>
        /// システム関連の処理
        /// </summary>
        virtual SystemInterface* VSystem() = 0;

    protected:
        /// <summary>
        /// モジュール開始
        /// </summary>
        virtual const Bool _VStart() override
        {
            HE_ASSERT(FALSE);
            return FALSE;
        }

        /// <summary>
        /// 前更新
        /// </summary>
        /// <param name="in_fDeltaTime"></param>
        /// <returns></returns>
        virtual const Bool _VBeforeUpdate(const Float32 in_fDeltaTime) override = 0;

        /// <summary>
        /// 更新
        /// </summary>
        /// <param name="in_fDeltaTime"></param>
        /// <returns></returns>
        virtual const Bool _VUpdate(const Float32 in_fDeltaTime) override = 0;

        /// <summary>
        /// 後更新
        /// </summary>
        /// <param name="in_fDeltaTime"></param>
        /// <returns></returns>
        virtual const Bool _VLateUpdate(const Float32 in_fDeltaTime) override = 0;
    };
}  // namespace Platform
