#pragma once

#include "CustomString.h"
#include "Engine/Core.h"

namespace Core::Common
{
    /// <summary>
    /// カスタムRTTI
    /// </summary>
    class RTTI
    {
        HE_CLASS_DEFAULT_CONSTRUCT_NG(RTTI);
        HE_CLASS_COPY_NG(RTTI);

    public:
        RTTI(const Char* in_szClass) : _szClass(in_szClass), _pBaseRTTI(NULL)
        {
            HE_LOG_LINE(HE_STR_TEXT("RTTI BaseClassName: %s"), in_szClass);
        }

        RTTI(const Char* in_szClass, const RTTI& in_rBaseRTTI)
            : _szClass(in_szClass), _pBaseRTTI(&in_rBaseRTTI)
        {
            HE_LOG_LINE(HE_STR_TEXT("RTTI InheritClassName: %s BaseClassName: %s"),
                        in_szClass, in_rBaseRTTI.GetName().Str());
        }

        const FixString128& GetName() const HE_NOEXCEPT { return this->_szClass; }
        Bool IsExactly(const RTTI& in_rRtti) const HE_NOEXCEPT
        {
            return (this->_pBaseRTTI == &in_rRtti);
        }
        Bool DerivesFrom(const RTTI*) const;

    private:
        const FixString128 _szClass;
        const RTTI* _pBaseRTTI = NULL;
    };
}  // namespace Core::Common
