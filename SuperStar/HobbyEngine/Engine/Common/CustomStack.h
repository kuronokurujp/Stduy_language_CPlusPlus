#pragma once

#include "Engine/Core.h"

namespace Core
{
    namespace Common
    {
        /// <summary>
        /// 固定長スタックの基本クラス
        /// </summary>
        template <class TYPE>
        class StackBase
        {
        public:
            StackBase(TYPE* in_tpArrayAddr, Uint32 in_uSize)
                : _pBuff(in_tpArrayAddr), _uCapacity(in_uSize)
            {
            }

            inline const Uint32 Capacity() const HE_NOEXCEPT { return this->_uCapacity; }
            inline const Uint32 Size() const HE_NOEXCEPT { return this->_uNum; }
            inline const Bool Empty() const HE_NOEXCEPT { return (this->_uNum <= 0); }

            void Clear() HE_NOEXCEPT { this->_uNum = 0; }

            void PushBack(const TYPE& in_rData)
            {
                HE_ASSERT(this->_uNum < this->Capacity());
                this->_pBuff[this->_uNum++] = in_rData;
            }

            TYPE PopBack()
            {
                Sint32 i = this->_uNum - 1;
                if (0 < this->_uNum) --this->_uNum;
                HE_ASSERT(0 <= i);

                i = HE_MAX(i, 0);
                return this->_pBuff[i];
            }

        private:
            TYPE* _pBuff      = NULL;
            Uint32 _uNum      = 0;
            Uint32 _uCapacity = 0;
        };

        /// <summary>
        /// 固定長の高速処理のスタック
        /// テンプレートで要素を決めている
        /// </summary>
        template <class TYPE, Uint32 CAPACITY>
        class CustomFixStack : public StackBase<TYPE>
        {
        public:
            CustomFixStack() : StackBase<TYPE>(_taBuff, CAPACITY) {}

        private:
            TYPE _taBuff[CAPACITY];
        };
    }  // namespace Common
}  // namespace Core
