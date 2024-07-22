// 以下のマクロ定義するとCatch2側でエントリーポイント(main)関数が定義される
#define CATCH_CONFIG_MAIN
#define HE_ENGINE_DEBUG

#include "ThirdParty/Catch.hpp"

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
#include "HobbyPlugin/Actor/ActorModule.cpp"
#include "HobbyPlugin/Actor/ActorModule.h"

// エンジン本体のテストコード
#include "HobbyEngine/TestEngine.hpp"

TEST_CASE("Plugin Test")
{
    // エンジン起動
    CREATE_HOBBY_ENGINE;

    HOBBY_ENGINE.Init();
    HOBBY_ENGINE.Start();

// エンジンのプラグインのテストコード
#include "HobbyPlugin/TestActorPlugin.hpp"

    DELETE_HOBBY_ENGINE;
}
