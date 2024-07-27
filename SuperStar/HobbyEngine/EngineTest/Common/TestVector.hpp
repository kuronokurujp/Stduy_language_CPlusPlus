#pragma once

#include "Engine/Common/CustomVector.h"

TEST_CASE("CustomVector Use")
{
    Core::Common::CustomFixVector<Sint32, 32> vector;

    CHECK(vector.Capacity() == 32);
    vector.PopBack();
    CHECK(vector.Size() == 0);

    vector.PushBack(10);
    CHECK(vector[0] == 10);
    vector.PushBack(20);
    CHECK(vector[1] == 20);
    vector.PushBack(30);
    CHECK(vector[2] == 30);
    vector.PushBack(40);
    CHECK(vector[3] == 40);
    CHECK(vector.Size() == 4);

    vector.Remove(20);
    CHECK(vector[2] == 40);
    CHECK(vector.Size() == 3);
    vector.PushBack(30);
    CHECK(vector[3] == 30);

    while (vector.Empty() == FALSE) vector.PopBack();

    CHECK(vector.Empty());
}
