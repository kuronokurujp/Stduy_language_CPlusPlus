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

        Bool VCreateMainWindow() override final;
        Bool VReleaseAllWindows() override final;

        /// <summary>
        /// やめる状態になっているか
        /// </summary>
        Bool VIsQuit() override final { return this->_bQuit; }

        Platform::TimeInterface* VTime() override final { return &this->_time; }
        Platform::InputInterface* VInput() override final { return &this->_input; }
        Platform::FileInterface* VFile() override final { return &this->_file; }
        Platform::ScreenInterface* VScreen() override final { return &this->_screen; }
        Platform::SystemInterface* VSystem() override final { return &this->_system; }

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        Bool _VStart() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        Bool _VRelease() override final;

        Bool _VBeforeUpdate(const Float32 in_fDeltaTime) override final;
        Bool _VUpdate(const Float32 in_fDeltaTime) override final;
        Bool _VLateUpdate(const Float32 in_fDeltaTime) override final;

    private:
        ::DXLib::Time _time;
        ::DXLib::Input _input;
        ::DXLib::File _file;
        ::DXLib::Screen _screen;
        ::DXLib::System _system;

        Bool _bQuit = FALSE;
    };

}  // namespace DXLib
