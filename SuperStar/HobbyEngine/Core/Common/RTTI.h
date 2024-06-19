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
            RTTI(const Char* in_className) :
                _className(in_className), _pBaseRTTI(NULL) {}

            RTTI(const Char* in_className, const RTTI& in_baseRTTI) :
                _className(in_className), _pBaseRTTI(&in_baseRTTI) {}

            const FixString128& GetName() const { return this->_className; }
            Bool IsExactly(const RTTI& in_rtti) const { return (this->_pBaseRTTI == &in_rtti); }
            Bool DerivesFrom(const RTTI& in_rtti) const;

        private:
            const FixString128 _className;
            const RTTI* _pBaseRTTI = NULL;
        };
    }
}
