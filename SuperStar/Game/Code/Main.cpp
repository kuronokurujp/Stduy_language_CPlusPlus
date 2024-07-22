// アプリのエントリーポイント

#include "Main.h"

#include "Engine.h"
#include "framework.h"

// 外部モジュール一覧
#include "ActorModule.h"
#include "AssetManagerModule.h"
#include "DXLibModule.h"
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
    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    // エンジンに設定したアプリを開始
    s_app.Start(HOBBY_ENGINE.IsDebugMode());

    // ゲームループ
    while (1)
    {
        if (HOBBY_ENGINE.BeforUpdateLoop() == FALSE) break;

        if (HOBBY_ENGINE.WaitFrameLoop() == FALSE) break;

        const Float32 d = HOBBY_ENGINE.GetDeltaTimeSec();

        // アプリメイン
        if (s_app.Update(d) == FALSE) break;

        if (HOBBY_ENGINE.MainUpdateLoop(d) == FALSE) break;

        if (HOBBY_ENGINE.AfterUpdateLoop(d) == FALSE) break;
    }

    s_app.End();

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
        MODULE_IMPORT(Platform);
        MODULE_IMPORT(Render);
        MODULE_IMPORT(Actor);
        MODULE_IMPORT(UI);
        MODULE_IMPORT(AssetManager);
        MODULE_IMPORT(Level);
        MODULE_IMPORT(Localization);
    }

    const Bool bInitRet = HOBBY_ENGINE.Start();
    HE_ASSERT(bInitRet && "初期化に失敗");

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
// #include "sequence/title/TitleSeq.h"
#endif

const Bool AppEntryGameMain::Start(const Bool in_bDebug)
{
    HE_LOG_LINE(HE_STR_TEXT("game start"));

    // TODO: ゲームのみで利用するライブラリを初期化
    //		LuaStateManager::Init();
    // リソースの起点ディレクトリを設定
    ModuleAssetManager()->SetMountDir(HE_STR_TEXT("Assets"));

    ModuleLocalization()->LoadSystemFile(
        Core::Common::FixString256(HE_STR_TEXT("Locate/System.toml")));
    ModuleLocalization()->LoadTextAll(Core::Common::FixString16(HE_STR_TEXT("JP")));

#ifdef HE_ENGINE_DEBUG
    if (in_bDebug)
    {
        // デバッグレベルを開始
        ModuleLevel()->GetManager()->StartLevel<Level::LevelLauncher>();
    }
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
