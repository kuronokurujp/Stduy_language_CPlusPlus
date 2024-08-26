#include "DXLibSystem.h"

#include "DxLib.h"

namespace DXLib
{
    const Uint32 System::GetRand(const Uint32 in_uMax)
    {
        return ::GetRand(in_uMax);
    }

    const Bool System::SetSeedRand(const Uint32 in_uVal)
    {
        return (::SRand(in_uVal) == 0);
    }
}  // namespace DXLib
