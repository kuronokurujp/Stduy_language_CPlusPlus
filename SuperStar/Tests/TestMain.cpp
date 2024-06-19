// 以下のマクロ定義するとCatch2側でエントリーポイント(main)関数が定義される
#define CATCH_CONFIG_MAIN
#define _HOBBY_ENGINE_DEBUG

#include "Catch.hpp"

// マクロテストコード
#include "Common/TestMacro.hpp"

// 簡易配列のテストコード
#include "Common/TestArray.hpp"
#include "Common/TestString.hpp"

// CMapテストコード
#include "Common/TestCMap.hpp"

// ランダム処理のテストコード
#include "Math/TestRandom.hpp"

// タスクのテストコード
#include "Task/TestTask.hpp"

// メモリ処理のテストコード
#include "Memory/TestMemory.hpp"

// プラグインのテストするためにプラグインのインクルードは特殊
// インクルードしたらプラグインを使える状態になる
// 使える状態にした後にエンジンを起動
#include "HobbyPlugin/Actor/ActorModule.h"
#include "HobbyPlugin/Actor/ActorModule.cpp"

// エンジン本体のテストコード
#include "HobbyEngine/TestEngine.hpp"

TEST_CASE("Plugin Test")
{
    // エンジン起動
    CREATE_HOBBY_ENGINE;

    HOBBY_ENGINE.PreInit();
    HOBBY_ENGINE.Init();

    // エンジンのプラグインのテストコード
    #include "HobbyPlugin/TestActorPlugin.hpp"

    HOBBY_ENGINE.End();

    RELEASE_HOBBY_ENGINE;
}


