// アプリのエントリーポイント

#include "framework.h"
#include "SuperStar.h"

#include "Engine.h"

// 外部モジュール一覧
#include "HobbyPlugin/DxLib/DXLibModule.h"
#include "HobbyPlugin/Render/RenderModule.h"
#include "HobbyPlugin/UI/UIModule.h"
#include "HobbyPlugin/Level/LevelModule.h"
#include "HobbyPlugin/Actor/ActorModule.h"
#include "HobbyPlugin/Localization/LocalizationModule.h"

#define MAX_LOADSTRING 100

// グローバル変数

// 現在のインターフェイス
static HINSTANCE hInst;

// タイトル バーのテキスト
static WCHAR szTitle[MAX_LOADSTRING];

// メイン ウィンドウ クラス名
static WCHAR szWindowClass[MAX_LOADSTRING];

// ゲームエントリーを宣言してエンジンに登録
static AppEntryGameMain entry;

// このコード モジュールに含まれる関数の宣言を転送します:
static BOOL InitInstance(HINSTANCE, int);
static void EndInstance(HINSTANCE hInstance);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // アプリに関連する情報を取得
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SUPERSTAR, szWindowClass, MAX_LOADSTRING);

    // アプリケーション初期化の実行:
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    // エンジン起動
    entry.Start(HOBBY_ENGINE.IsDebugMode());

    // ゲームループ
    while(1)
    {
        if (HOBBY_ENGINE.BeforUpdateLoop() == FALSE)
            break;

        if (HOBBY_ENGINE.WaitFrameLoop() == FALSE)
            break;

        const Float32 d = HOBBY_ENGINE.GetDeltaTimeSec();

        // アプリメイン
        if (entry.Update(d) == FALSE)
            break;

        if (HOBBY_ENGINE.MainUpdateLoop(d) == FALSE)
            break;

        if (HOBBY_ENGINE.AfterUpdateLoop(d) == FALSE)
            break;
    }

    entry.End();

    EndInstance(hInstance);

    return 0;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    // グローバル変数にインスタンス ハンドルを格納する
    hInst = hInstance;

    // TODO: DxLibプラットフォーム特有の設定をする
    START_HOBBY_ENGINE

    const Bool bPreInitRet = HOBBY_ENGINE.PreInit();
    E_ASSERT(bPreInitRet && "事前初期化に失敗");

    // TODO: 必要な外部モジュールを追加
    {
        DXLib::DxLibModule* pDxLibModule = HOBBY_ENGINE.ModuleManager().Add<DXLib::DxLibModule>();

        HOBBY_ENGINE.ModuleManager().Add<Render::RenderModule>();
        HOBBY_ENGINE.ModuleManager().Add<UI::UIModule>();
        HOBBY_ENGINE.ModuleManager().Add<Actor::ActorModule>();
        HOBBY_ENGINE.ModuleManager().Add<Level::LevelModule>();
        HOBBY_ENGINE.ModuleManager().Add<Localization::LocalizationModule>();
    }

    const Bool bInitRet = HOBBY_ENGINE.Init();
    E_ASSERT(bInitRet && "初期化に失敗");

    return TRUE;
}

void EndInstance(HINSTANCE hInstance)
{
    HOBBY_ENGINE.End();
}

// アプリの起動エントリークラス

#ifdef _HOBBY_ENGINE_DEBUG

// 最初に実行するレベルをインクルード
#include "Level/Debug/LevelDebugMain.h"

#else
// #include "sequence/title/TitleSeq.h"
#endif

const Bool AppEntryGameMain::Start(const Bool in_bDebug)
{
    E_LOG(E_STR_TEXT("game start"));
    // TODO: ゲームのみで利用するライブラリを初期化
    //		LuaStateManager::Init();
    Localization::LocalizationModule::I().LoadSystemFile(E_STR_TEXT("Locate/System.toml"));

#ifdef _HOBBY_ENGINE_DEBUG
    if (in_bDebug)
    {
        // デバッグレベルを開始
        //HOBBY_ENGINE.StartLevel<Level::DebugMain>();
        HOBBY_ENGINE.GetLevelModule().GetManager()->StartLevel<Level::DebugMain>();
    }
#endif

    return TRUE;
}

const Bool AppEntryGameMain::Update(const Float32 in_deltaTime)
{
    //E_LOG(E_STR_TEXT("game update"));
        /*
        if( mp_parent->update() == false )
        {
            return false;
        }
#ifdef	_DEBUG
        if( isButtonTriggerOn( 'w' ) )
        {
            //	デバッグ画面に強制的に戻る
            SAFE_DELETE( mp_parent );
            mp_parent = new MainSeq::C_Parent( new MainSeq::C_DebugSeq() );
        }

        if( isButtonTriggerOn( 't' ) )
        {
            const bool bEnable	= LuaStateManager::IsDebugEnable();
            if( bEnable )
            {
                LuaStateManager::SetDebugEnable( false );
            }
            else
            {
                LuaStateManager::SetDebugEnable( true );
            }
        }
#endif
*/

    return TRUE;
}

const Bool AppEntryGameMain::End()
{
    E_LOG(E_STR_TEXT("game end"));
    /*
    LuaStateManager::Final();
    SAFE_DELETE( mp_parent );
    */

    return TRUE;
}

