#pragma once

#include "MiniEngine.h"

#include "Core/Common/Singleton.h"
#include "Core/Common/Handle.h"
#include "Core/Common/FixArray.h"

#include "Module/Module.h"

// テスト実行用にcppファイルが必要
#ifdef CATCH_CONFIG_MAIN
#include "Module/Module.cpp"
#endif

// エンジン標準利用のモジュール一覧
#include "HobbyPlugin/Level/LevelModule.h"

// 前方宣言
namespace Core
{
    namespace Time
    {
        class FPS;
    }
}

/// <summary>
/// ゲームエンジン本体
/// インスタンスは一つのみなのでシングルトンにする
/// </summary>
class Engine : public Core::Common::Singleton<Engine>
{
public:
    /// <summary>
    /// 事前初期化
    /// メモリシステムなどエンジンを動かす上で最低元の初期化をする
    /// </summary>
    /// <returns></returns>
    const Bool PreInit();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <returns></returns>
    const Bool Init();

    /// <summary>
    /// 終了
    /// </summary>
    void End();

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
    const Bool MainUpdateLoop(const Float32 in_deltaSec);
    const Bool AfterUpdateLoop(const Float32 in_deltaSec);

    /// <summary>
    /// メモリ管理を取得
    /// </summary>
    /// <returns></returns>
    Core::Memory::Manager& MemManager() { return this->_memoryManager; }

    /// <summary>
    /// モジュール管理を取得
    /// </summary>
    /// <returns></returns>
    Module::ModuleManager& ModuleManager() const { return *this->_pModuleManager; }

    /// <summary>
    /// レベルモジュールを取得
    /// </summary>
    /// <returns></returns>
    Level::LevelModule& GetLevelModule();

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
    const Float32 GetDeltaTimeSec() const;

private:
    /// <summary>
    /// Colisions this instance.
    /// </summary>
    /// <returns></returns>
    void _Colision() {}

    /// <summary>
    /// 処理結果を出力.
    /// </summary>
    void _Redner();

private:
    Bool _bPreInit = FALSE;
    Bool _bInit = FALSE;

    // メモリ管理
    Core::Memory::Manager _memoryManager;

    // fPS 制御
    std::shared_ptr<Core::Time::FPS> _pFPS = NULL;

    // TODO: モジュール管理
    Module::ModuleManager* _pModuleManager = NULL;
};

// エンジン参照マクロ
#define HOBBY_ENGINE Engine::I()

// エンジン作成
#define CREATE_HOBBY_ENGINE static Engine s_engine
// エンジン解放
#define RELEASE_HOBBY_ENGINE Engine::I().Reset()

// カレントレベル参照マクロ
#define HOBBY_CURRENT_LEVEL (*HOBBY_ENGINE.GetLevelModule().GetManager()->CurrentLevel())


