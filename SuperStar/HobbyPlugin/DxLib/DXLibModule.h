#pragma once

// プラットフォームモジュールを使ってDxLibのプラットフォームモジュールを作成
#include "Platform/PlatformModule.h"

// DxLibのシステム一覧
// モジュールのヘッダーファイルは全てインクルードする
#include "DXLibTime.h"
#include "DXLibInput.h"
#include "DxLibFile.h"

namespace DXLib
{
    /// <summary>
    /// DxLibプラットフォームが利用できるモジュール
    /// </summary>
    class DxLibModule final : public Platform::PlatformModule
    {
    public:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Init() override final;

        /// <summary>
        /// モジュール終了
        /// </summary>
        /// <returns></returns>
        const Bool End() override final;

        const bool CreateMainWindow() override final;
        const bool ReleaseAllWindows() override final;

        const bool BeforUpdate(const Float32 in_deltaTime) override final;
        const bool Update(const Float32 in_deltaTime) override final;
        const bool AfterUpdate(const Float32 in_deltaTime) override final;

        /// <summary>
        /// 時間システム
        /// </summary>
        /// <returns></returns>
        Platform::TimeSystemInterface* Time() override final { return &this->_time; }

        /// <summary>
        /// 入力システム
        /// </summary>
        /// <returns></returns>
        Platform::InputSystemInterface* Input() override final { return &this->_input; }

        /// <summary>
        /// ファイルシステム
        /// </summary>
        /// <returns></returns>
        Platform::FileSystemInterface* File() override final { return &this->_file; }

        // 描画
        void BeginRender(void* in_pCmdBuff) override final;
        void Redner(void* in_pCmdBuff) override final;
        void EndRender(void* in_pCmdBuff) override final;

    private:
        TimeSystem _time;
        InputSystem _input;
        FileSystem _file;
    };
}

MODULE_GENRATE_DECLARATION(DXLib::DxLibModule, DxLib);
