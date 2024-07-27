#pragma once

#include "Engine/Core.h"

/// <summary>
/// 配列制御クラス
/// 配列制御をクラスで行う事ができる
/// Uint32 nums[32]
/// 上記のC++標準の配列を使うのではなく配列制御クラスを使って配列を扱うようにしてほしい
/// Uint32 nums[32]
/// ↓
/// CustomArray<Uint32, 32> nums;
/// </summary>
namespace Core
{
    namespace Common
    {
        /// <summary>
        /// 固定長配列の基本クラス
        /// </summary>
        template <class TYPE>
        class ArrayBase
        {
            HE_CLASS_COPY_CONSTRUCT_NG(ArrayBase);
            HE_CLASS_MOVE_CONSTRUCT_NG(ArrayBase);

        public:
            ArrayBase(TYPE* in_pArrayAddr, Uint32 in_uSize)
                : _pBuff(in_pArrayAddr), _uCapacity(in_uSize)
            {
            }

            inline const Uint32 Capacity() const HE_NOEXCEPT { return this->_uCapacity; }

            /// <summary>
            /// 指定した要素に値をコピーして設定
            /// </summary>
            inline void Set(const Uint32 in_uIndex, const TYPE& in_data) HE_NOEXCEPT
            {
                HE_ASSERT(in_uIndex < this->_uCapacity);
                this->_pBuff[in_uIndex] = in_data;
            }

            TYPE& operator[](const Uint32 in_uIndex) const
            {
                HE_ASSERT(0 < this->_uCapacity);
                return this->_pBuff[in_uIndex];
            }

        private:
            TYPE* _pBuff      = NULL;
            Uint32 _uCapacity = 0;
        };

        /// <summary>
        /// 固定長配列
        /// テンプレートで要素を決めている
        /// </summary>
        template <class TYPE, Uint32 CAPACITY>
        class CustomArray : public ArrayBase<TYPE>
        {
        public:
            CustomArray() : ArrayBase<TYPE>(this->_aBuff, CAPACITY) {}

        private:
            TYPE _aBuff[CAPACITY];
        };
    }  // namespace Common
}  // namespace Core
