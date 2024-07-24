#pragma once

#include "Engine/Macro.h"
#include "Engine/Str.h"

TEST_CASE("Macro Text")
{
    // 上限値の制御
    CHECK(HE_MAX(3, 4) == 4);
    // 下限値の制御
    CHECK(HE_MIN(3, 4) == 3);

    // 上限値の制御
    CHECK(HE_MAX(15, 10) == 15);
    // 下限値の制御
    CHECK(HE_MIN(15, 10) == 10);

    // 上限値の制御
    CHECK(HE_MAX(1, 1) == 1);
    // 下限値の制御
    CHECK(HE_MIN(1, 1) == 1);

    HE_LOG(HE_STR_TEXT("test"));
}
