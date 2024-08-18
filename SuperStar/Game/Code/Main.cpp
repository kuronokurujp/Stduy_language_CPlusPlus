// アプリのエントリーポイント

#include "Main.h"

#include "Engine/Engine.h"
#include "framework.h"

// 外部モジュール一覧
#include "ActorModule.h"
#include "AssetManagerModule.h"
#include "DXLibModule.h"
#include "EnhancedInputModule.h"
#include "LevelModule.h"
#include "LocalizationModule.h"
#include "RenderModule.h"
#include "UIModule.h"

#define MAX_LOADSTRING 100

// グローバル変数

// 現在のインターフェイス
static HINSTANCE hInst;

// タイトル バーのテキスト
static WCHAR szTitle[MAX_LOADSTRING];

// メイン ウィンドウ クラス名
static WCHAR szWindowClass[MAX_LOADSTRING];

// ゲームエンジンに登録するアプリインスタンス
static AppEntryGameMain s_app;

// このコード モジュールに含まれる関数の宣言を転送します:
static BOOL InitInstance(HINSTANCE, int);
static void EndInstance(HINSTANCE hInstance);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // アプリに関連する情報を取得
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SUPERSTAR, szWindowClass, MAX_LOADSTRING);

    // アプリケーション初期化の実行:
    if (InitInstance(hInstance, nCmdShow))
    {
        // エンジンに設定したアプリを開始
        s_app.Start(HOBBY_ENGINE.IsDebugMode());

        // ゲームループ
        while (HOBBY_ENGINE.IsAppQuit() == FALSE)
        {
            // 前処理
            {
                const Float32 d = HOBBY_ENGINE.GetDeltaTimeSec();
                if (HOBBY_ENGINE.BeforeUpdateLoop(d) == FALSE) break;
            }

            if (HOBBY_ENGINE.WaitFrameLoop() == FALSE) break;

            {
                const Float32 d = HOBBY_ENGINE.GetDeltaTimeSec();

                // アプリメイン
                if (s_app.Update(d) == FALSE) break;

                if (HOBBY_ENGINE.MainUpdateLoop(d) == FALSE) break;

                if (HOBBY_ENGINE.LateUpdateLoop(d) == FALSE) break;
            }
        }

        s_app.End();
    }

    EndInstance(hInstance);

    return 0;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    // グローバル変数にインスタンス ハンドルを格納する
    hInst = hInstance;

    // エンジン起動
    CREATE_HOBBY_ENGINE;

    const Bool bPreInitRet = HOBBY_ENGINE.Init();
    HE_ASSERT(bPreInitRet && "事前初期化に失敗");

    // 利用プラグインをインポート
    {
        MODULE_IMPORT(DXLib::DXLibModule);
        MODULE_IMPORT(Render::RenderModule);
        MODULE_IMPORT(Actor::ActorModule);
        MODULE_IMPORT(UI::UIModule);
        MODULE_IMPORT(AssetManager::AssetManagerModule);
        MODULE_IMPORT(Level::LevelModule);
        MODULE_IMPORT(Localization::LocalizationModule);
        MODULE_IMPORT(EnhancedInput::EnhancedInputModule);
    }

    const Bool bInitRet = HOBBY_ENGINE.Start();
    HE_ASSERT(bInitRet && "初期化に失敗");

    // ゲームウィンドウを作成
    if (HOBBY_ENGINE.CreateGameWindow() == FALSE) return FALSE;

    return TRUE;
}

void EndInstance(HINSTANCE hInstance)
{
    DELETE_HOBBY_ENGINE;
}

// アプリの起動エントリークラス

#ifdef HE_ENGINE_DEBUG

// 最初に実行するレベルをインクルード
#include "Level/LevelLauncher.h"

#else
#include "Level/LevelTitle.h"
#endif

const Bool AppEntryGameMain::Start(const Bool in_bDebug)
{
    HE_LOG_LINE(HE_STR_TEXT("game start"));

    // ユーザー共通入力割り当て設定
    {
        EnhancedInput::ActionData::ActionKeyMap aKeys(
            {Platform::EKeyboard::EKeyboard_A});
        EnhancedInput::ActionData::ActionTouchMap aTouchs(
            {Platform::EInputMouseType::EInputMouseType_Left});

        EnhancedInput::ActionMap mInputAction;
        mInputAction.Add(HE_STR_TEXT("UIButton"), EnhancedInput::ActionData(aKeys, aTouchs));

        auto pInputModule = Module::ModuleManager::I().Get<EnhancedInput::EnhancedInputModule>();
        pInputModule->SetCommonMappingAction(mInputAction);
    }

    // TODO: ゲームのみで利用するライブラリを初期化
    //		LuaStateManager::Init();

    // リソースの起点ディレクトリを設定
    auto pAssetManagerModule = Module::ModuleManager::I().Get<AssetManager::AssetManagerModule>();
    pAssetManagerModule->SetMountDir(HE_STR_TEXT("Assets"));

    auto pLocateModule = Module::ModuleManager::I().Get<Localization::LocalizationModule>();
    pLocateModule->LoadSystemFile(Core::Common::FixString256(HE_STR_TEXT("Locate/System.toml")));
    pLocateModule->LoadTextAll(Core::Common::FixString16(HE_STR_TEXT("JP")));

    auto pLevelModule = Module::ModuleManager::I().Get<Level::LevelModule>();
#ifdef HE_ENGINE_DEBUG
    // デバッグレベルを開始
    pLevelModule->GetManager()->StartLevel<Level::LevelLauncher>();
#else
    pLevelModule->GetManager()->StartLevel<Level::LevelTitle>();
#endif

    return TRUE;
}

const Bool AppEntryGameMain::Update(const Float32 in_fDeltaTime)
{
    return TRUE;
}

const Bool AppEntryGameMain::End()
{
    HE_LOG_LINE(HE_STR_TEXT("game end"));
    return TRUE;
}
