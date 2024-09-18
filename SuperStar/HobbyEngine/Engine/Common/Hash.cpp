#include "Hash.h"

namespace Core::Common
{
    // 65536未満で最大の素数
    static const Uint32 uBase = 65521L;

    // NMAXは 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1を満たすnの最大数
    static const Uint32 uNMax = 5552;

    Uint64 HashName(const Char* in_szIdent)
    {
        if (in_szIdent == NULL) return 0;

#define DO1(buf, i)                   \
    {                                 \
        s1 += HE_CHAR_LOWWER(buf[i]); \
        s2 += s1;                     \
    }

#define DO2(buf, i) \
    DO1(buf, i);    \
    DO1(buf, i + 1);

#define DO4(buf, i) \
    DO2(buf, i);    \
    DO2(buf, i + 2);

#define DO8(buf, i) \
    DO4(buf, i);    \
    DO4(buf, i + 4);

#define DO16(buf) \
    DO8(buf, 0);  \
    DO8(buf, 8);

        if (HE_STR_CMP(in_szIdent, HE_STR_TEXT("*")) == 0) return 0;

        Uint32 s1 = 0;
        Uint32 s2 = 0;

        Uint32 k = 0;
        for (Uint32 len = static_cast<Uint32>(HE_STR_LEN(in_szIdent)); 0 < len;)
        {
            k = len < uNMax ? len : uNMax;
            len -= k;

            while (k == 16)
            {
                DO16(in_szIdent);
                in_szIdent += 16;
                k -= 16;
            }

            if (k != 0)
            {
                do
                {
                    s1 += *in_szIdent++;
                    s2 += s1;
                } while (--k);

                s1 %= uBase;
                s2 %= uBase;
            }
        }

#pragma warning(push)
#pragma warning(disable : 4312)

        return static_cast<Uint64>((s2 << 16) | s1);

#pragma warning(pop)
#undef DO1
#undef DO2
#undef DO4
#undef DO8
#undef DO16
    }

}  // namespace Core::Common
