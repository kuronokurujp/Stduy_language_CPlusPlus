#pragma once

#include "Engine/Common/CustomArray.h"

TEST_CASE("Test Array Use")
{
    Core::Common::CustomArray<Sint32, 32> arr;

    CHECK(arr.Capacity() == 32);

    arr.Set(0, 10);
    CHECK(arr[0] == 10);

    arr.Set(1, 20);
    CHECK(arr[1] == 20);
    arr.Set(2, 30);
    CHECK(arr[2] == 30);
    arr.Set(3, 40);
    CHECK(arr[3] == 40);

    arr.Set(0, 5);
    CHECK(arr[0] == 5);
}
