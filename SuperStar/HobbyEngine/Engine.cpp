#include "MiniEngine.h"
#include "Engine.h"

#include "GameMain.h"
#include "Core/Memory/Memory.h"
#include "Core/Time/FPS.h"

// テスト実行用にcppファイルが必要
#ifdef CATCH_CONFIG_MAIN
#include "Core/Time/FPS.cpp"
#endif

// エンジン標準利用のモジュール一覧
#include "HobbyPlugin/Render/RenderModule.h"

#include "Platform/PlatformModule.h"

/// <summary>
/// 事前初期化
/// </summary>
/// <returns></returns>
const Bool Engine::PreInit()
{
    E_ASSERT(this->_bPreInit == FALSE);
    if (this->_bPreInit)
        return TRUE;

    E_LOG_LINE(E_STR_TEXT("エンジンの前準備"));

    // カスタムメモリ確保
    // TODO: 確保数は適当にしている
    if (this->_memoryManager.Init(1024 * 1024 * 300) == FALSE)
    {
        E_ASSERT(0 && "カスタムメモリの初期化に失敗");
        return FALSE;
    }

    // メモリページ確保
    // TODO: 確保量は適当にしている
    {
        // メモリサイズのイニシャライズ
        Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = 
        {
            {
                // メモリページNo
                0,
                // 利用するメモリ数
                1024 * 1024 * 300
            },
        };

        if (this->_memoryManager.SetupMemoryPage(memoryPageSetupInfoArray, E_ARRAY_NUM(memoryPageSetupInfoArray)) == FALSE)
        {
            E_ASSERT(0 && "カスタムメモリのページ作成に失敗");
            return FALSE;
        }

        E_ASSERT(this->_memoryManager.CheckAllMemoryBlock());
    }

    MODULE_MANAGER_DEFINITION;
    this->_pModuleManager = &Module::ModuleManager::I();
    E_ASSERT(this->_pModuleManager && "モジュール管理がない");

    this->_bPreInit = TRUE;

    // 成功
    return TRUE;
}

/// <summary>
/// 初期化
/// </summary>
/// <returns></returns>
const Bool Engine::Init()
{
    E_ASSERT(this->_bInit == FALSE);
    if (this->_bInit)
        return TRUE;

    E_LOG_LINE(E_STR_TEXT("エンジンの初期化"));

    // 追加したモジュール適応
    this->_pModuleManager->Apply();

    // FPSタイマーを作成
    // ゲームを固定フレームレートにするため
    if (Platform::PlatformModule::Have())
    {
        Core::Time::FPS* pFPS = new Core::Time::FPS(Platform::PlatformModule::I().Time());
        E_ASSERT(pFPS);

        this->_pFPS = std::shared_ptr<Core::Time::FPS>(pFPS);
    }

    this->_bInit = TRUE;

    // 成功
    return TRUE;
}

/// <summary>
/// 終了
/// </summary>
void Engine::End()
{
    E_ASSERT(this->_bPreInit);
    E_ASSERT(this->_bInit);

    if (this->_bInit == FALSE)
        return;

    if (this->_bPreInit == FALSE)
        return;

    // 確保したメモリを解放しないとEngineのデストラクタでエラーになる
    if (this->_pModuleManager != NULL)
        this->_pModuleManager->Release();

    this->_pFPS.reset();

    // アプリ/エンジンで利用しているメモリはここで解放
    // 確保したメモリが残っている場合はエラーになるので注意
    this->_memoryManager.End();

    E_LOG_LINE(E_STR_TEXT("エンジンの終了"));
}

/// <summary>
/// ゲームウィンドウ生成.
/// </summary>
/// <returns></returns>
const Bool Engine::CreateGameWindow()
{
    E_ASSERT(this->_bInit);

    if (Platform::PlatformModule::Have() == FALSE)
        return FALSE;

    // windowを作成
    if (Platform::PlatformModule::I().CreateMainWindow() == FALSE)
        return FALSE;

    return TRUE;
}

/// <summary>
/// ゲームウィンドウを解放.
/// </summary>
/// <returns></returns>
void Engine::ReleseGameWindow()
{
    if (Platform::PlatformModule::Have() == FALSE)
        return;

    Platform::PlatformModule::I().ReleaseAllWindows();
}

const Bool Engine::BeforUpdateLoop()
{
    if (Platform::PlatformModule::Have() == FALSE)
        return FALSE;

    if (Platform::PlatformModule::I().BeforUpdate(this->_pFPS->GetDeltaTimeSec()) == FALSE)
        return FALSE;

    return TRUE;
}

const Bool Engine::WaitFrameLoop()
{
    if (Platform::PlatformModule::Have() == FALSE)
        return FALSE;

    // 1 / 60 秒経過しないと更新しない
    if (this->_pFPS != NULL)
    {
        while (this->_pFPS->UpdateWait(16))
        {
            Platform::PlatformModule::I().Time()->SleepMSec(1);
        }
    }

    return TRUE;
}

const Bool Engine::MainUpdateLoop(const Float32 in_deltaSec)
{
    // モジュール更新
    if (this->_pModuleManager != NULL)
        this->_pModuleManager->Update(in_deltaSec);

    return TRUE;
}

const Bool Engine::AfterUpdateLoop(const Float32 in_deltaSec)
{
    // 描画処理
    this->_Redner();

    if (Platform::PlatformModule::Have() == FALSE)
        return FALSE;

    if (Platform::PlatformModule::I().AfterUpdate(this->_pFPS->GetDeltaTimeSec()) == FALSE)
        return FALSE;

    return TRUE;
}

Level::LevelModule& Engine::GetLevelModule()
{
    return Level::LevelModule::I();
}

const Float32 Engine::GetDeltaTimeSec() const
{
    if (this->_pFPS == NULL)
        return 0.0f;

    return this->_pFPS->GetDeltaTimeSec();
}

/// <summary>
/// Generates the output.
/// </summary>
void Engine::_Redner()
{
    if (Platform::PlatformModule::Have() == FALSE)
        return;

    if (Render::RenderModule::Have() == FALSE)
        return;

    // 描画コマンドを実行
    Platform::PlatformModule& platformModule = Platform::PlatformModule::I();
    Render::RenderModule& renderModule = Render::RenderModule::I();

    auto pComArray = reinterpret_cast<void*>(renderModule.GetCmdBuff());
    E_ASSERT(pComArray);

    // 描画コマンドを渡す
    platformModule.BeginRender(pComArray);
    platformModule.Redner(pComArray);
    platformModule.EndRender(pComArray);

    // 描画コマンドをクリアする
    renderModule.ClearCmd();
}

