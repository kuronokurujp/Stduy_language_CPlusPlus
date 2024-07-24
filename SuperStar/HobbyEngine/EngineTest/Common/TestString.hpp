#pragma once

#include "Engine/Common/CustomString.h"

TEST_CASE("FixString Use")
{
    Core::Common::FixString<32> str(HE_STR_TEXT("test"));
    // テキストが存在するか
    CHECK_FALSE(str.Empty());

    // 意図したテキストが存在するか
    CHECK(str == HE_STR_TEXT("test"));

    // 文字数が意図通りか
    CHECK(str.Length() == 4);

    // 文字位置判定が意図通りか
    CHECK(str.Find(HE_STR_TEXT("t")) == 0);
    CHECK(str.Find(HE_STR_TEXT("s")) == 2);

    // 日本語が使えるか
    Core::Common::FixString<128> str2(HE_STR_TEXT("日本語"));
    CHECK(str2 == HE_STR_TEXT("日本語"));

    // 文字数が意図通りか
    CHECK(str2.Length() == 3);

    // 文字位置判定が意図通りか
    CHECK(str2.Find(HE_STR_TEXT("日")) == 0);
}
