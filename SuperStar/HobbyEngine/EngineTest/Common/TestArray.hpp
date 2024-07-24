#pragma once

#include "Engine/Common/CustomArray.h"

TEST_CASE("FastFixArray Use")
{
    Core::Common::FastFixArray<Sint32, 32> arr;

    CHECK(arr.Capacity() == 32);
    CHECK(arr.Size() == 0);
    CHECK(arr.Empty() == TRUE);

    arr.PushBack(10);
    CHECK(arr.Size() == 1);
    CHECK(arr.Empty() == FALSE);

    arr.RemoveAt(0);
    CHECK(arr.Size() == 0);
    CHECK(arr.Empty() == TRUE);

    arr.PushBack(10);
    arr.PushBack(20);
    arr.PushBack(30);
    arr.PushBack(40);
    CHECK(arr.Size() == 4);

    CHECK(arr[0] == 10);
    arr.RemoveAt(0);
    CHECK(arr[0] == 20);
    CHECK(arr[1] == 30);
    CHECK(arr[2] == 40);
}
