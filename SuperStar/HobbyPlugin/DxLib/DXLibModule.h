#pragma once

// プラットフォームモジュールを使ってDxLibのプラットフォームモジュールを作成
#include "Engine/Platform/PlatformModule.h"

// DxLibのシステム一覧
// モジュールのヘッダーファイルは全てインクルードする
#include "DxLib/DXLibFile.h"
#include "DxLib/DXLibInput.h"
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

        const bool CreateMainWindow() override final;
        const bool ReleaseAllWindows() override final;

        /// <summary>
        /// やめる状態になっているか
        /// </summary>
        /// <returns></returns>
        const Bool IsQuit() override final { return this->_bQuit; }

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
        const Bool _Start() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual const Bool _Release() override final;

        const bool _BeforeUpdate(const Float32 in_fDeltaTime) override final;
        const bool _Update(const Float32 in_fDeltaTime) override final;
        const bool _LateUpdate(const Float32 in_fDeltaTime) override final;

    private:
        TimeSystem _time;
        InputSystem _input;
        FileSystem _file;

        Bool _bQuit = FALSE;
    };

}  // namespace DXLib
