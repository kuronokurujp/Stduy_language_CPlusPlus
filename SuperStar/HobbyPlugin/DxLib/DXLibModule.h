#pragma once

// プラットフォームモジュールを使ってDxLibのプラットフォームモジュールを作成
#include "Engine/Platform/PlatformModule.h"

// DxLibのシステム一覧
// モジュールのヘッダーファイルは全てインクルードする
#include "DxLib/DXLibFile.h"
#include "DxLib/DXLibInput.h"
#include "DxLib/DXLibScreen.h"
#include "DxLib/DXLibSystem.h"
#include "DxLib/DXLibTime.h"

namespace DXLib
{
    /// <summary>
    /// DxLibプラットフォームが利用できるモジュール
    /// 特殊モジュール
    /// プラットフォームモジュールを継承した特殊モジュール
    /// 一つしか継承できない
    /// モジュール参照名は基本クラスにある
    /// </summary>
    class DXLibModule final : public Platform::PlatformModule
    {
    public:
        DXLibModule();

        const Bool CreateMainWindow() override final;
        const Bool ReleaseAllWindows() override final;

        /// <summary>
        /// やめる状態になっているか
        /// </summary>
        const Bool IsQuit() override final { return this->_bQuit; }

        Platform::TimeInterface* Time() override final { return &this->_time; }
        Platform::InputInterface* Input() override final { return &this->_input; }
        Platform::FileInterface* File() override final { return &this->_file; }
        Platform::ScreenInterface* Screen() override final { return &this->_screen; }
        Platform::SystemInterface* System() override final { return &this->_system; }

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool _Start() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        const Bool _Release() override final;

        const Bool _BeforeUpdate(const Float32 in_fDeltaTime) override final;
        const Bool _Update(const Float32 in_fDeltaTime) override final;
        const Bool _LateUpdate(const Float32 in_fDeltaTime) override final;

    private:
        ::DXLib::Time _time;
        ::DXLib::Input _input;
        ::DXLib::File _file;
        ::DXLib::Screen _screen;
        ::DXLib::System _system;

        Bool _bQuit = FALSE;
    };

}  // namespace DXLib
