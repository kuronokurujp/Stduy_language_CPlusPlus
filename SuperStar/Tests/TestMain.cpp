﻿// 以下のマクロ定義するとCatch2側でエントリーポイント(main)関数が定義される
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
