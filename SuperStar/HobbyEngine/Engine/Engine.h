#pragma once

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
class Engine : public Core::Common::Singleton<Engine>
{
public:
    /// <summary>
    /// 初期化
    /// メモリシステムなどエンジンを動かす上で最低元の初期化をする
    /// </summary>
    /// <returns></returns>
    const Bool Init();

    /// <summary>
    /// エンジン起動
    /// Initメソッドを事前に呼ばないとエラーになる
    /// </summary>
    /// <returns></returns>
    const Bool Start();

    const Bool Release() override final;

    /// <summary>
    /// ゲームウィンドウ生成
    /// </summary>
    /// <returns></returns>
    const Bool CreateGameWindow();

    /// <summary>
    /// ゲームウィンドウを解放
    /// </summary>
    /// <returns></returns>
    void ReleseGameWindow();

    // エンジンを稼働させるためのループ用メソッド
    const Bool BeforUpdateLoop();
    const Bool WaitFrameLoop();
    const Bool MainUpdateLoop(const Float32 in_fDeltaSec);
    const Bool AfterUpdateLoop(const Float32 in_fDeltaSec);

    /// <summary>
    /// メモリ管理を取得
    /// </summary>
    /// <returns></returns>
    Core::Memory::Manager& MemManager() { return this->_memoryManager; }

    /// <summary>
    /// モジュール管理を取得
    /// </summary>
    /// <returns></returns>
    Module::ModuleManager& ModuleManager() { return this->_moduleManager; }

    /// <summary>
    /// プラットフォームのモジュールを取得
    /// windows / android / iosなどのプラットフォームが扱える予定
    /// 現在はwindowsのみ
    /// </summary>
    /// <returns></returns>
    Platform::PlatformModule* GetPlatformModule();

    /// <summary>
    /// デバッグモードかどうか
    /// </summary>
    /// <returns></returns>
    inline const Bool IsDebugMode() const
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
    const Float32 GetDeltaTimeSec();

private:
    /// <summary>
    /// Colisions this instance.
    /// </summary>
    /// <returns></returns>
    void _Colision() {}

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
