#pragma once

// プラットフォームモジュールを使ってDxLibのプラットフォームモジュールを作成
#include "Platform/PlatformModule.h"

// DxLibのシステム一覧
// モジュールのヘッダーファイルは全てインクルードする
#include "DXLibInput.h"
#include "DXLibTime.h"
#include "DxLibFile.h"

namespace DXLib
{
    /// <summary>
    /// DxLibプラットフォームが利用できるモジュール
    /// </summary>
    class DxLibModule final : public Platform::PlatformModule
    {
    public:
        DxLibModule(const Char* in_szName) : PlatformModule(in_szName) {}

        const bool CreateMainWindow() override final;
        const bool ReleaseAllWindows() override final;

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

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Start() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual const Bool Release() override final;

        const bool BeforUpdate(const Float32 in_fDeltaTime) override final;
        const bool Update(const Float32 in_fDeltaTime) override final;
        const bool AfterUpdate(const Float32 in_fDeltaTime) override final;

        // 描画
        void BeginRender(void* in_pCmdBuff) override final;
        void Redner(void* in_pCmdBuff) override final;
        void EndRender(void* in_pCmdBuff) override final;

    private:
        TimeSystem _time;
        InputSystem _input;
        FileSystem _file;
    };
}  // namespace DXLib

MODULE_GENRATE_DECLARATION(DXLib::DxLibModule, Platform);
