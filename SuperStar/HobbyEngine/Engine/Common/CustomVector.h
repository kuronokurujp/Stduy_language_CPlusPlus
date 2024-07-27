#pragma once

#include "Engine/Core.h"

namespace Core
{
    namespace Common
    {
        /// <summary>
        /// Vectorの基本クラス
        /// </summary>
        template <class TYPE>
        class VectorBase
        {
        public:
            VectorBase(TYPE* in_tpArrayAddr, Uint32 in_uSize)
                : _pBuff(in_tpArrayAddr), _uCapacity(in_uSize)
            {
            }

            inline const Uint32 Capacity() const HE_NOEXCEPT { return this->_uCapacity; }
            inline const Uint32 Size() const HE_NOEXCEPT { return this->_uNum; }
            inline const Bool Empty() const HE_NOEXCEPT { return (this->_uNum <= 0); }

            void Clear() HE_NOEXCEPT { this->_uNum = 0; }

            /// <summary>
            /// 配列の末尾に要素を追加
            /// クラスや構造体だとコピー処理が走るのでコピー処理ができるクラスのみ使える
            /// </summary>
            /// <param name="in_data"></param>
            /// <returns></returns>
            TYPE& PushBack(const TYPE& in_data)
            {
                HE_ASSERT(this->_uNum < this->Capacity());
                // コピー処理が発生
                this->_pBuff[this->_uNum++] = in_data;

                return this->_pBuff[this->_uNum - 1];
            }

            void PopBack()
            {
                if (0 < this->_uNum) --this->_uNum;
            }

            /// <summary>
            /// 引数の要素と一致したのを削除
            /// クラスの場合だと==の定義が必要
            /// </summary>
            /// <param name="in_tData"></param>
            /// <returns></returns>
            const Bool Remove(const TYPE& in_data)
            {
                Bool bRet = FALSE;
                // 要素が重複している可能性があるので全要素チェック
                for (Uint32 i = 0; i < this->_uNum; ++i)
                {
                    if (this->_pBuff[i] == in_data)
                    {
                        this->RemoveAt(i);
                        bRet = TRUE;
                    }
                }

                return bRet;
            }

            /// <summary>
            /// 配列のループ処理中に削除する時に使える
            /// </summary>
            /// <param name="in_uIndex"></param>
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
                    ::memmove(&this->_pBuff[in_uIndex], &this->_pBuff[in_uIndex + 1], uSize);
                }
                else
                {
                    // 末尾の削除は要素数を減らすだけでよい
                }

                --this->_uNum;
            }

            TYPE& operator[](const Uint32 in_uIndex) const
            {
                HE_ASSERT(0 < this->_uNum);
                return this->_pBuff[in_uIndex];
            }

            TYPE* GetPtr(const Uint32 in_uIndex) const
            {
                HE_ASSERT(0 < this->_uNum);
                return &this->_pBuff[in_uIndex];
            }

        private:
            TYPE* _pBuff      = NULL;
            Uint32 _uNum      = 0;
            Uint32 _uCapacity = 0;
        };

        /// <summary>
        /// 固定長のカスタムVector
        /// テンプレートで要素を決めている
        /// </summary>
        template <class TYPE, Uint32 CAPACITY>
        class CustomFixVector : public VectorBase<TYPE>
        {
        public:
            CustomFixVector() : VectorBase<TYPE>(this->_aBuff, CAPACITY) {}

        private:
            TYPE _aBuff[CAPACITY];
        };

    }  // namespace Common
}  // namespace Core
