#pragma once

#include "Core/Core.h"
#include <memory>

namespace Core
{
    namespace Common
    {
        /// <summary>
        /// 固定長配列の基本クラス
        /// 削除すると配列の要素とインデックスの結びつけが変わるのでインデックスは保有していはいけない
        /// </summary>
        /// <typeparam name="TYPE"></typeparam>
        template<class TYPE>
        class FastFixArrayBase
        {
        public:
            FastFixArrayBase(TYPE* in_pArray, Uint32 in_size) : _pDataArray(in_pArray), _max(in_size) {}

            inline const Uint32 Max() const E_NOEXCEPT { return this->_max; }
            inline const Uint32 Size() const E_NOEXCEPT { return this->_num; }
            inline const Uint32 IsEmpty() const E_NOEXCEPT { return (this->_num <= 0); }

            void Empty()
            {
                this->_num = 0;
            }

            void PushBack(const TYPE in_data)
            {
                E_ASSERT(this->_num < this->Max());
                this->_pDataArray[this->_num++] = in_data;
            }

            TYPE PopBack() E_NOEXCEPT
            {
                Sint32 i = this->_num;
                if (this->_num > 0)
                    this->_num--;

                i = E_MAX(i, 0);
                return this->_pDataArray[i];
            }

            void Remove(const TYPE in_data)
            {
                for (Uint32 i = 0; i < this->_num; ++i)
                {
                    if (this->_pDataArray[i] == in_data)
                    {
                        this->RemoveAt(i);
                        return;
                    }
                }
            }

            void RemoveAt(const Uint32 in_index)
            {
                E_ASSERT(in_index < this->_num);

                Uint32 lastIndex = this->_num - 1;
                if (in_index < lastIndex)
                {
                    // 削除する要素位置に上書きして削除
                    // メモリ移動のみで高速削除できる
                    Uint32 size = (lastIndex - in_index) * sizeof(TYPE);
                    ::memmove(&this->_pDataArray[in_index], &this->_pDataArray[in_index + 1], size);
                }
                else
                {
                    // 末尾の削除は要素数を減らすだけでよい
                }

                --this->_num;
            }

            TYPE Back()
            {
                E_ASSERT(0 < this->_num);

                Uint32 index = this->_num - 1;
                --this->_num;

                return this->_pDataArray[index];
            }

            TYPE operator[](const Uint32 in_index) const
            {
                E_ASSERT(0 < this->_num);
                return this->_pDataArray[in_index];
            }

            TYPE* GetPtr(const Uint32 in_index) const
            {
                E_ASSERT(0 < this->_num);
                return &this->_pDataArray[in_index];
            }
            
        private:
            TYPE* _pDataArray;
            Uint32 _num = 0;
            Uint32 _max = 0;
        };

        /// <summary>
        /// 固定長の高速処理の配列
        /// テンプレートで要素を決めている
        /// </summary>
        /// <typeparam name="TYPE"></typeparam>
        /// <typeparam name="SIZE"></typeparam>
        template<class TYPE, Uint32 SIZE>
        class FastFixArray : public FastFixArrayBase<TYPE>
        {
        public:
            FastFixArray() : FastFixArrayBase<TYPE>(_dataArray, SIZE) {}

        private:
            TYPE _dataArray[SIZE];
        };
    }
}
