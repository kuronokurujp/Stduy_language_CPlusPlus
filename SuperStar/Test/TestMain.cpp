// 以下のマクロ定義するとCatch2側でエントリーポイント(main)関数が定義される
// #define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_RUNNER
#define HE_ENGINE_DEBUG

#include "Engine/Engine.h"
#include "ThirdParty/Catch.hpp"

// 外部モジュール一覧
#include "ActorModule.h"
#include "AssetManagerModule.h"
#include "DXLibModule.h"
#include "LevelModule.h"
#include "LocalizationModule.h"
#include "RenderModule.h"
#include "UIModule.h"

// Catch.hppのカスタムメイン関数を実装
#ifndef __OBJC__

#ifndef CATCH_INTERNAL_CDECL
#ifdef _MSC_VER
#define CATCH_INTERNAL_CDECL __cdecl
#else
#define CATCH_INTERNAL_CDECL
#endif
#endif

#if defined(CATCH_CONFIG_WCHAR) && defined(CATCH_PLATFORM_WINDOWS) && defined(_UNICODE) && \
    !defined(DO_NOT_USE_WMAIN)
// Standard C/C++ Win32 Unicode wmain entry point
int CATCH_INTERNAL_CDECL wmain(int argc, wchar_t* argv[], wchar_t*[])
{
#else
// Standard C/C++ main entry point
int CATCH_INTERNAL_CDECL main(int argc, char* argv[])
{
#endif

    const int ret = Catch::Session().run(argc, argv);

    return ret;
}

#else  // __OBJC__

// Objective-C entry point
int main(int argc, char* const argv[])
{
#if !CATCH_ARC_ENABLED
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
#endif

    Catch::registerTestMethods();
    int result = Catch::Session().run(argc, (char**)argv);

#if !CATCH_ARC_ENABLED
    [pool drain];
#endif

    return result;
}

#endif  // __OBJC__

// マクロテストコード
#include "EngineTest/Common/TestMacro.hpp"

// カスタム配列のテストコード
#include "EngineTest/Common/TestArray.hpp"
// カスタムVectorのテストコード
#include "EngineTest/Common/TestVector.hpp"
// カスタムStackのテストコード
#include "EngineTest/Common/TestStack.hpp"
// 文カスタム字列のテストコード
#include "EngineTest/Common/TestString.hpp"
// カスタムリストのテストコード
#include "EngineTest/Common/TestList.hpp"

// Mapテストコード
#include "EngineTest/Common/TestMap.hpp"

// ランダム処理のテストコード
#include "EngineTest/Math/TestRandom.hpp"

// タスクのテストコード
#include "EngineTest/Task/TestTask.hpp"

// メモリ処理のテストコード
#include "EngineTest/Memory/TestMemory.hpp"

// プラグインのテストするためにプラグインのインクルードは特殊
// インクルードしたらプラグインを使える状態になる
// 使える状態にした後にエンジンを起動
#include "ActorModule.h"

// エンジン本体のテストコード
#include "EngineTest/TestEngine.hpp"
