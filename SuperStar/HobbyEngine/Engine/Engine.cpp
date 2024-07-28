#include "Engine.h"

#include "Engine/Memory/Memory.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformModule.h"
#include "Engine/Time/FPS.h"
#include "GameMain.h"

/// <summary>
/// 事前初期化
/// </summary>
/// <returns></returns>
const Bool Engine::Init()
{
    HE_ASSERT(this->_bInit == FALSE);
    if (this->_bInit) return TRUE;

    HE_LOG_LINE(HE_STR_TEXT("エンジンの前準備"));

    // メモリ管理
    // カスタムメモリ確保
    // TODO: 確保数は適当にしている
    if (this->_memoryManager.Start(1024 * 1024 * 300) == FALSE)
    {
        HE_ASSERT(0 && "カスタムメモリの初期化に失敗");
        return FALSE;
    }

    // メモリページ確保
    // TODO: 確保量は適当にしている
    {
        // メモリサイズのイニシャライズ
        Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
            {// メモリページNo
             0,
             // 利用するメモリ数
             1024 * 1024 * 300},
        };

        if (this->_memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                                 HE_ARRAY_NUM(memoryPageSetupInfoArray)) == FALSE)
        {
            HE_ASSERT(0 && "カスタムメモリのページ作成に失敗");
            return FALSE;
        }

        HE_ASSERT(this->_memoryManager.CheckAllMemoryBlock());
    }

    this->_bInit = TRUE;

    // 成功
    return TRUE;
}

/// <summary>
/// 初期化
/// </summary>
/// <returns></returns>
const Bool Engine::Start()
{
    HE_ASSERT(this->_bStart == FALSE);
    if (this->_bStart) return TRUE;

    HE_LOG_LINE(HE_STR_TEXT("エンジンの初期化"));

    // 追加したモジュール適応
    this->_moduleManager.Start();

    // FPSタイマーを作成
    // ゲームを固定フレームレートにするため
    auto pPlatformModule = this->GetPlatformModule();
    if (pPlatformModule)
    {
        this->_spFPS = Core::Memory::MakeCustomShared<Core::Time::FPS>(pPlatformModule->Time());
    }

    this->_bStart = TRUE;

    // 成功
    return TRUE;
}

const Bool Engine::Release()
{
    HE_ASSERT(this->_bInit);
    HE_ASSERT(this->_bStart);

    if (this->_bStart == FALSE) return TRUE;

    if (this->_bInit == FALSE) return TRUE;

    // 確保したメモリを解放しないとEngineのデストラクタでエラーになる
    this->_moduleManager.Release();

    this->_spFPS.reset();

    // アプリ/エンジンで利用しているメモリはここで解放
    // 確保したメモリが残っている場合はエラーになるので注意
    this->_memoryManager.Release();

    HE_LOG_LINE(HE_STR_TEXT("エンジンの終了"));
    // デストラクタが呼ばれる
    // メモリ管理から取得したメモリを参照すると例外エラーになる

    return TRUE;
}

/// <summary>
/// ゲームウィンドウ生成.
/// </summary>
/// <returns></returns>
const Bool Engine::CreateGameWindow()
{
    HE_ASSERT(this->_bStart);

    auto pPlatform = this->GetPlatformModule();
    if (pPlatform == NULL) return FALSE;

    // windowを作成
    if (pPlatform->CreateMainWindow() == FALSE) return FALSE;

    return TRUE;
}

/// <summary>
/// ゲームウィンドウを解放.
/// </summary>
/// <returns></returns>
void Engine::ReleseGameWindow()
{
    auto pPlatform = this->GetPlatformModule();
    if (pPlatform == NULL) return;

    pPlatform->ReleaseAllWindows();
}

const Bool Engine::BeforUpdateLoop()
{
    auto pPlatform = this->GetPlatformModule();
    if (pPlatform == NULL) return FALSE;

    if (pPlatform->BeforUpdate(this->_spFPS->GetDeltaTimeSec(pPlatform->Time())) == FALSE)
        return FALSE;

    // 描画の前準備
    pPlatform->BeginRender();

    return TRUE;
}

const Bool Engine::WaitFrameLoop()
{
    auto pPlatform = this->GetPlatformModule();
    if (pPlatform == NULL) return FALSE;

    // 1 / 60 秒経過しないと更新しない
    // TODO: FPS設定できるようにした方がいい
    if (this->_spFPS != NULL)
    {
        while (this->_spFPS->UpdateWait(pPlatform->Time(), 16))
        {
            // TODO: 待機する時間を正確に計算した方がいい
            pPlatform->Time()->SleepMSec(1);
        }

        // HE_LOG_LINE(HE_STR_TEXT("%d"), this->_spFPS->GetFrameRate());
    }

    return TRUE;
}

const Bool Engine::MainUpdateLoop(const Float32 in_fDeltaSec)
{
    // モジュール更新
    this->_moduleManager.Update(in_fDeltaSec);

    // 描画処理
    auto pPlatformModule = this->GetPlatformModule();
    if (pPlatformModule != NULL) pPlatformModule->Redner();

    return TRUE;
}

const Bool Engine::AfterUpdateLoop(const Float32 in_fDeltaSec)
{
    auto pPlatform = this->GetPlatformModule();
    if (pPlatform == NULL) return FALSE;

    if (pPlatform->AfterUpdate(this->_spFPS->GetDeltaTimeSec(pPlatform->Time())) == FALSE)
        return FALSE;

    // 描画終了
    pPlatform->EndRender();

    return TRUE;
}

Platform::PlatformModule* Engine::GetPlatformModule()
{
    auto pPlatform = reinterpret_cast<Platform::PlatformModule*>(
        this->_moduleManager.Get(HE_STR_TEXT("Platform")));

    return pPlatform;
}

const Float32 Engine::GetDeltaTimeSec()
{
    if (this->_spFPS == NULL) return 0.0f;

    auto pPlatform = this->GetPlatformModule();
    if (pPlatform == NULL) return 0.0f;

    return this->_spFPS->GetDeltaTimeSec(pPlatform->Time());
}
