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

    // 値のループ
    CHECK(HE_LOOP_IN_RANGE(4, 0, 3) == 0);
    CHECK(HE_LOOP_IN_RANGE(3, 0, 3) == 3);
    CHECK(HE_LOOP_IN_RANGE(0, 0, 3) == 0);
    CHECK(HE_LOOP_IN_RANGE(1, 0, 3) == 1);
    CHECK(3.2f <= HE_LOOP_IN_RANGE(0.3f, 0.34f, 3.5f));
    CHECK(HE_LOOP_IN_RANGE(0.35f, 0.34f, 3.5f) == 0.35f);
    CHECK(HE_LOOP_IN_RANGE(3.6f, 0.34f, 3.5f) <= 0.44f);
}
