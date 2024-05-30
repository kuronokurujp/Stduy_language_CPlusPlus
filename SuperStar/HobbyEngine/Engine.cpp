#include "MiniEngine.h"
#include "Engine.h"

#include "GameMain.h"
#include "Core/Memory/Memory.h"
#include "Core/Time/FPS.h"

// エンジン標準利用のモジュール一覧
#include "HobbyPlugin/Render/RenderModule.h"
#include "HobbyPlugin/Actor/ActorModule.h"

#include "HobbyPlugin/Actor/Actor.h"

#include "Platform/PlatformModule.h"

/// <summary>
/// 事前初期化
/// </summary>
/// <returns></returns>
const Bool Engine::PreInit()
{
    if (this->_bPreInit)
        return FALSE;

    // カスタムメモリ確保
    // TODO: 確保数は適当にしている
    if (this->_memoryManager.Init(1024 * 1024 * 300) == FALSE)
    {
        E_ASSERT(0 && "カスタムメモリの初期化に失敗");
        return FALSE;
    }

    // ページ確保テスト
    // TODO: 確保量は適当にしている
    {
        // メモリサイズのイニシャライズ
        Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = 
        {
            { 0, 1024 * 1024 * 300 },
        };

        if (this->_memoryManager.SetupMemoryPage(memoryPageSetupInfoArray, E_ARRAY_NUM(memoryPageSetupInfoArray)) == FALSE)
        {
            E_ASSERT(0 && "カスタムメモリのページ作成に失敗");
            return FALSE;
        }

        E_ASSERT(this->_memoryManager.CheckAllMemoryBlock());
    }

    // モジュール管理はすぐに利用するので事前初期化で用意する
    {
        this->_pModuleManager = new Module::ModuleManager();
        if (this->_pModuleManager->Init() == FALSE)
        {
            E_ASSERT(0 && "モジュール管理の作成に失敗");
            return FALSE;
        }
    }

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
    if (this->_bInit)
        return FALSE;

    // TODO: 追加したモジュール適応
    this->_pModuleManager->Apply();

    // 必ず利用するモジュールがあるかチェック
    // TODO: なければNullモジュール扱うにするべきかも
    {
        if (Platform::PlatformModule::I().Have() == FALSE)
        {
            E_ASSERT(0 && "プラットフォームモジュールが存在しない");
            return FALSE;
        }

        if (Render::RenderModule::I().Have() == FALSE)
        {
            E_ASSERT(0 && "描画モジュールが存在しない");
            return FALSE;
        }

        if (Level::LevelModule::I().Have() == FALSE)
        {
            E_ASSERT(0 && "レベルモジュールが存在しない");
            return FALSE;
        }

        if (Actor::ActorModule::I().Have() == FALSE)
        {
            E_ASSERT(0 && "アクターモジュールが存在しない");
            return FALSE;
        }
    }

    // FPSタイマーを作成
    // ゲームを固定フレームレートにするため
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
    if (this->_bInit == FALSE)
        return;

    if (this->_bPreInit == FALSE)
        return;

    // 確保したメモリを解放しないとEngineのデストラクタでエラーになる
    this->_pModuleManager->End();

    this->_pFPS.reset();

    E_SAFE_DELETE(this->_pModuleManager);

    // アプリ/エンジンで利用しているメモリはここで解放
    // 確保したメモリが残っている場合はエラーになるので注意
    this->_memoryManager.End();
}

/// <summary>
/// ゲームウィンドウ生成.
/// </summary>
/// <returns></returns>
const Bool Engine::CreateGameWindow()
{
    E_ASSERT(this->_bInit);

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
    Platform::PlatformModule::I().ReleaseAllWindows();
}

const Bool Engine::BeforUpdateLoop()
{
    if (Platform::PlatformModule::I().BeforUpdate(this->_pFPS->GetDeltaTimeSec()) == FALSE)
        return FALSE;

    return TRUE;
}

const Bool Engine::WaitFrameLoop()
{
    // 1 / 60 秒経過しないと更新しない
    while (this->_pFPS->UpdateWait(16))
    {
        Platform::PlatformModule::I().Time()->SleepMSec(1);
    }

    return TRUE;
}

const Bool Engine::MainUpdateLoop(const Float32 in_deltaSec)
{
    // モジュール更新
    this->_pModuleManager->Update(in_deltaSec);

    return TRUE;
}

const Bool Engine::AfterUpdateLoop(const Float32 in_deltaSec)
{
    // 描画処理
    this->_Redner();

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
    return this->_pFPS->GetDeltaTimeSec();
}

/// <summary>
/// Generates the output.
/// </summary>
void Engine::_Redner()
{
    // TODO: 描画コマンドを実行
    Platform::PlatformModule& platformModule = Platform::PlatformModule::I();
    Render::RenderModule& renderModule = Render::RenderModule::I();

    auto pComArray = reinterpret_cast<void*>(renderModule.GetCmdBuff());
    E_ASSERT(pComArray);

    // TODO: 描画コマンドを渡す
    platformModule.BeginRender(pComArray);
    platformModule.Redner(pComArray);
    platformModule.EndRender(pComArray);

    // TODO: 描画コマンドをクリアする
    renderModule.ClearCmd();
}
