#pragma once

#include "Engine/Core.h"

namespace Core
{
    namespace Common
    {
        /// <summary>
        /// 固定長配列の基本クラス
        /// 削除すると配列の要素とインデックスの結びつけが変わるのでインデックスは保有していはいけない
        /// </summary>
        template <class TYPE>
        class FastFixArrayBase
        {
        public:
            FastFixArrayBase(TYPE* in_tpArrayAddr, Uint32 in_uSize)
                : _tpBuff(in_tpArrayAddr), _uCapacity(in_uSize)
            {
            }

            inline const Uint32 Capacity() const HE_NOEXCEPT { return this->_uCapacity; }
            inline const Uint32 Size() const HE_NOEXCEPT { return this->_uNum; }
            inline const Bool Empty() const HE_NOEXCEPT { return (this->_uNum <= 0); }

            void Clear() HE_NOEXCEPT { this->_uNum = 0; }

            void PushBack(const TYPE in_tData)
            {
                HE_ASSERT(this->_uNum < this->Capacity());
                this->_tpBuff[this->_uNum++] = in_tData;
            }

            TYPE PopBack()
            {
                Sint32 i = this->_uNum - 1;
                if (this->_uNum > 0) --this->_uNum;

                i = HE_MAX(i, 0);
                return this->_tpBuff[i];
            }

            const Bool Remove(const TYPE in_tData)
            {
                for (Uint32 i = 0; i < this->_uNum; ++i)
                {
                    if (this->_tpBuff[i] == in_tData)
                    {
                        this->RemoveAt(i);
                        return TRUE;
                    }
                }

                return FALSE;
            }

            void RemoveAt(const Uint32 in_uIndex)
            {
                HE_ASSERT(in_uIndex < this->_uNum);
                HE_ASSERT(0 < this->_uNum);

                Uint32 uLastIndex = this->_uNum - 1;
                if (in_uIndex < uLastIndex)
                {
                    // 削除する要素位置に上書きして削除
                    // メモリ移動のみで高速削除できる
                    Uint32 uSize = (uLastIndex - in_uIndex) * sizeof(TYPE);
                    ::memmove(&this->_tpBuff[in_uIndex], &this->_tpBuff[in_uIndex + 1], uSize);
                }
                else
                {
                    // 末尾の削除は要素数を減らすだけでよい
                }

                --this->_uNum;
            }

            TYPE Back()
            {
                HE_ASSERT(0 < this->_uNum);

                Uint32 uIndex = this->_uNum - 1;
                --this->_uNum;

                return this->_tpBuff[uIndex];
            }

            TYPE operator[](const Uint32 in_uIndex) const
            {
                HE_ASSERT(0 < this->_uNum);
                return this->_tpBuff[in_uIndex];
            }

            TYPE* GetPtr(const Uint32 in_uIndex) const
            {
                HE_ASSERT(0 < this->_uNum);
                return &this->_tpBuff[in_uIndex];
            }

        private:
            TYPE* _tpBuff     = NULL;
            Uint32 _uNum      = 0;
            Uint32 _uCapacity = 0;
        };

        /// <summary>
        /// 固定長の高速処理の配列
        /// テンプレートで要素を決めている
        /// </summary>
        template <class TYPE, Uint32 CAPACITY>
        class FastFixArray : public FastFixArrayBase<TYPE>
        {
        public:
            FastFixArray() : FastFixArrayBase<TYPE>(_taBuff, CAPACITY) {}

        private:
            TYPE _taBuff[CAPACITY];
        };
    }  // namespace Common
}  // namespace Core
