#pragma once

#include "Engine/Common/UUID.h"
#include "Engine/Core.h"

TEST_CASE("Macro UUID 32Bit")
{
    static const Uint32 s_uUUIDCheckCount = 100000;
    Uint32 uHitCount                      = 0;
    Uint32 uCount                         = 0;
    Uint32 uaUUIDValue[s_uUUIDCheckCount] = {0};
    for (Uint32 i = 0; i < s_uUUIDCheckCount; ++i)
    {
        Uint32 uValue = Core::Common::GenerateUUIDBy32Bit();
        for (Uint32 j = 0; j < uCount; ++j)
        {
            if (uValue == uaUUIDValue[j])
            {
                ++uHitCount;
                break;
            }
        }

        uaUUIDValue[i] = uValue;
        ++uCount;
    }

    // 100000回繰り返してUUIDを生成すると一度だけ値が衝突した
    HE_LOG_LINE(HE_STR_TEXT("32bitのUUID生成を(%d)回繰り返した結果, 値の衝突回数(%d)だった"),
                s_uUUIDCheckCount, uHitCount);
}
