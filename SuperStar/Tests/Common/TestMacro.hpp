#pragma once

#include "Core/Macro.h"
#include "Core/Str.h"

TEST_CASE("Macro Text")
{
    // 上限値の制御
    CHECK(E_MAX(3, 4) == 4);
    // 下限値の制御
    CHECK(E_MIN(3, 4) == 3);

    // 上限値の制御
    CHECK(E_MAX(15, 10) == 15);
    // 下限値の制御
    CHECK(E_MIN(15, 10) == 10);

    // 上限値の制御
    CHECK(E_MAX(1, 1) == 1);
    // 下限値の制御
    CHECK(E_MIN(1, 1) == 1);

    E_LOG(E_STR_TEXT("test"));
}
