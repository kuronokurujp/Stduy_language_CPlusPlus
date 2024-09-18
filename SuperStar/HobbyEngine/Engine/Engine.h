﻿#pragma once

#include "Engine/Common/CustomArray.h"
#include "Engine/Common/Handle.h"
#include "Engine/Common/Singleton.h"
#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// 前方宣言

namespace Core::Time
{
    class FPS;
}

namespace Platform
{
    class PlatformModule;
}

/// <summary>
/// ゲームエンジン本体
/// インスタンスは一つのみなのでシングルトンにする
/// </summary>
class Engine final : public Core::Common::Singleton<Engine>
{
public:
    /// <summary>
    /// 初期化
    /// メモリシステムなどエンジンを動かす上で最低元の初期化をする
    /// </summary>
    Bool Init();

    /// <summary>
    /// モジュールを作成
    /// </summary>
    template <class T>
    Bool CreateModule()
    {
        HE_ASSERT(this->_bInit);

        T* pModule = HE_NEW_LAST(T, 0);
        if (this->_moduleManager.RegistHeapModule(pModule) == FALSE)
        {
            HE_DELETE(pModule);
            return FALSE;
        }

        return TRUE;
    }

    /// <summary>
    /// エンジン起動
    /// Initメソッドを事前に呼ばないとエラーになる
    /// </summary>
    Bool Start();
    Bool VRelease() override final;

    /// <summary>
    /// メインゲームウィンドウ生成
    /// </summary>
    Bool CreateMainWindow();

    /// <summary>
    /// ゲームウィンドウを解放
    /// </summary>
    void ReleseWindows();

    // エンジンを稼働させるためのループ用メソッド
    Bool BeforeUpdateLoop(const Float32 in_fDeltaSec);
    Bool WaitFrameLoop();
    Bool MainUpdateLoop(const Float32 in_fDeltaSec);
    Bool LateUpdateLoop(const Float32 in_fDeltaSec);

    /// <summary>
    /// メモリ管理を取得
    /// </summary>
    Core::Memory::Manager& MemManager() { return this->_memoryManager; }

    /// <summary>
    /// モジュール管理を取得
    /// </summary>
    /// <returns></returns>
    Module::ModuleManager& ModuleManager() { return this->_moduleManager; }

    /// <summary>
    /// デバッグモードかどうか
    /// </summary>
    /// <returns></returns>
    inline Bool IsDebugMode() const
    {
#ifdef _DEBUG
        return TRUE;
#endif
        return FALSE;
    }

    /// <summary>
    /// １フレームの差分時間を秒で取得
    /// </summary>
    /// <returns></returns>
    Float32 GetDeltaTimeSec();

    /// <summary>
    /// アプリを辞める状態か
    /// </summary>
    /// <returns></returns>
    Bool IsAppQuit();

private:
    /// <summary>
    /// Colisions this instance.
    /// </summary>
    /// <returns></returns>
    void _Colision() {}

    /// <summary>
    /// プラットフォームのモジュールを取得
    /// windows / android / iosなどのプラットフォームが扱える予定
    /// 現在はwindowsのみ
    /// </summary>
    /// <returns></returns>
    Platform::PlatformModule* _PlatformModule();

private:
    Bool _bInit  = FALSE;
    Bool _bStart = FALSE;

    // メモリ管理
    Core::Memory::Manager _memoryManager;

    // fPS 制御
    std::shared_ptr<Core::Time::FPS> _spFPS = NULL;

    // モジュール管理
    Module::ModuleManager _moduleManager;
};

// エンジン参照マクロ
#define HOBBY_ENGINE Engine::I()

// エンジン作成
#define CREATE_HOBBY_ENGINE static Engine s_engine
// エンジン削除
#define DELETE_HOBBY_ENGINE Engine::I().Reset()
