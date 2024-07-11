#pragma once

#include "Core/Core.h"
#include "FixString.h"

namespace Core
{
    namespace Common
    {
        /// <summary>
        /// カスタムRTTI
        /// </summary>
        class RTTI
        {
            E_CLASS_DEFAULT_CONSTRUCT_NG(RTTI);
            E_CLASS_COPY_CONSTRUCT_NG(RTTI);

        public:
            RTTI(const Char* in_pClassName) : _className(in_pClassName), _pBaseRTTI(NULL)
            {
                E_LOG_LINE(E_STR_TEXT("RTTI BaseClassName: ") E_STR_FORMAT_TEXT, in_pClassName);
            }

            RTTI(const Char* in_pClassName, const RTTI& in_baseRTTI)
                : _className(in_pClassName), _pBaseRTTI(&in_baseRTTI)
            {
                E_LOG_LINE(E_STR_TEXT("RTTI InheritClassName: ")
                               E_STR_FORMAT_TEXT E_STR_TEXT(" BaseClassName: ") E_STR_FORMAT_TEXT,
                           in_pClassName, in_baseRTTI.GetName().Str());
            }

            const FixString128& GetName() const E_NOEXCEPT { return this->_className; }
            Bool IsExactly(const RTTI& in_rtti) const E_NOEXCEPT
            {
                return (this->_pBaseRTTI == &in_rtti);
            }
            Bool DerivesFrom(const RTTI& in_rtti) const;

        private:
            const FixString128 _className;
            const RTTI* _pBaseRTTI = NULL;
        };
    }  // namespace Common
}  // namespace Core
