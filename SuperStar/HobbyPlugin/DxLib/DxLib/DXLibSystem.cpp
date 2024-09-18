#include "DXLibSystem.h"

#include "DxLib.h"

namespace DXLib
{
    Uint32 System::VGetRand(const Uint32 in_uMax)
    {
        return ::GetRand(in_uMax);
    }

    Bool System::VSetSeedRand(const Uint32 in_uVal)
    {
        return (::SRand(in_uVal) == 0);
    }
}  // namespace DXLib
