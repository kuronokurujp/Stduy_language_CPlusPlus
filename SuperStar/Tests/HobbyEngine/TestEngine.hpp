#pragma once

#include "Engine.cpp"
#include "Engine.h"

/// <summary>
/// 初期化と終了が成功するか
/// </summary>
TEST_CASE("Engine Init to End")
{
    // エンジン起動
    CREATE_HOBBY_ENGINE;

    // エンジンが存在しているか
    CHECK(HOBBY_ENGINE.Have());

    CHECK(HOBBY_ENGINE.PreInit());
    CHECK(HOBBY_ENGINE.PreInit());

    CHECK(HOBBY_ENGINE.Init());
    CHECK(HOBBY_ENGINE.Init());

    HOBBY_ENGINE.End();

    // エンジン終了
    RELEASE_HOBBY_ENGINE;
}
