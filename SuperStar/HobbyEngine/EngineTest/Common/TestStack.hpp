#pragma once

#include "Engine/Common/CustomStack.h"

TEST_CASE("Test Stack Use")
{
    Core::Common::CustomFixStack<Sint32, 32> stack;

    CHECK(stack.Capacity() == 32);
    CHECK(stack.Empty());

    stack.PushBack(10);
    CHECK_FALSE(stack.Empty());
    CHECK(stack.PopBack() == 10);
    CHECK(stack.Empty());

    stack.PushBack(10);
    stack.PushBack(10);
    stack.PushBack(10);
    stack.PushBack(10);
    stack.PushBack(10);
    stack.PushBack(10);
    CHECK_FALSE(stack.Empty());
    CHECK(stack.Size() == 6);

    stack.PopBack();
    stack.PopBack();
    stack.PopBack();
    stack.PopBack();
    CHECK(stack.Size() == 2);

    stack.Clear();
    CHECK(stack.Empty());
}
