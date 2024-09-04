#pragma once

#include "Engine/Common/Hash.h"
#include "Engine/Macro.h"

// ハッシュ値がかぶっていないか
TEST_CASE("Test Hash Duplication Check")
{
    const Uint32 uCount          = 3;
    const Char* aszTexts[uCount] = {HE_STR_TEXT("a"), HE_STR_TEXT("b"), HE_STR_TEXT("c")};
    Uint64 auuHash[uCount]       = {};

    HE_LOG_LINE(HE_STR_EMPTY);
    for (auto i = 0; i < HE_ARRAY_NUM(aszTexts); ++i)
    {
        auuHash[i] = Core::Common::HashName(aszTexts[i]);
        HE_LOG_LINE(HE_STR_TEXT("text: %s to hash: %lld %s"), aszTexts[i], auuHash[i],
                    HE_STR_TEXT("日本"));
    }

    for (auto i = 0; i < HE_ARRAY_NUM(aszTexts); ++i)
    {
        auto hash = auuHash[i];
        for (auto j = i + 1; j < HE_ARRAY_NUM(aszTexts); ++j)
        {
            CHECK(hash != auuHash[j]);
        }
    }
}
