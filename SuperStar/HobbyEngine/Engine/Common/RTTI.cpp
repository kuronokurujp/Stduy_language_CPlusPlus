#include "RTTI.h"

namespace Core::Common
{
    Bool RTTI::DerivesFrom(const RTTI* in_pRtti) const
    {
        const RTTI* pCompare = this;
        while (pCompare != NULL)
        {
            if (pCompare == in_pRtti) return TRUE;

            pCompare = pCompare->_pBaseRTTI;
        }

        return FALSE;
    }
}  // namespace Core::Common
