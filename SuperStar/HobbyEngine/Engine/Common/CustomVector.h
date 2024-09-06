#pragma once

#include "Engine/Core.h"

namespace Core::Common
{
    /// <summary>
    /// Vectorの基本クラス
    /// </summary>
    template <class TYPE>
    class VectorBase
    {
        HE_CLASS_COPY_CONSTRUCT_NG(VectorBase);

    public:
        VectorBase(TYPE* in_tpArrayAddr, Uint32 in_uSize)
            : _pBuff(in_tpArrayAddr), _uCapacity(in_uSize)
        {
        }

        virtual ~VectorBase() { this->Clear(); }

        inline const Uint32 Capacity() const HE_NOEXCEPT { return this->_uCapacity; }
        inline const Uint32 Size() const HE_NOEXCEPT { return this->_uSize; }
        inline const Bool Empty() const HE_NOEXCEPT { return (this->_uSize <= 0); }

        void Clear() HE_NOEXCEPT { this->_uSize = 0; }

        /// <summary>
        /// 配列の末尾に要素を追加
        /// クラスや構造体だとコピー処理が走るのでコピー処理ができるクラスのみ使える
        /// </summary>
        /// <param name="in_data"></param>
        /// <returns></returns>
        TYPE& PushBack(const TYPE& in_rData)
        {
            HE_ASSERT(this->_uSize < this->Capacity());
            // コピー処理が発生
            this->_pBuff[this->_uSize] = in_rData;
            ++this->_uSize;

            return this->_pBuff[this->_uSize - 1];
        }

        void PopBack()
        {
            if (0 < this->_uSize) --this->_uSize;
        }

        /// <summary>
        /// 指定インデックスにデータ挿入
        /// 挿入するとデータの配置が変化するので使う側はインデックスを取得するのはNG
        /// </summary>
        TYPE& Insert(const Uint32 in_uIndex, const TYPE& in_rData)
        {
            HE_ASSERT(this->_uSize < this->Capacity());

            // データ末尾位置より後ろに挿入ならPushでいい
            if (this->_uSize <= in_uIndex) return this->PushBack(in_rData);

            // データ末尾位置より前に挿入
            // 配置したデータを一つ後ろにずらす
            const Uint32 uSize = (this->_uSize - in_uIndex) * sizeof(TYPE);
            ::memmove(&this->_pBuff[in_uIndex + 1], &this->_pBuff[in_uIndex], uSize);

            // 後ろに一つずらした事で挿入先が空いたので挿入
            this->_pBuff[in_uIndex] = in_rData;
            ++this->_uSize;

            return this->_pBuff[in_uIndex];
        }

        /// <summary>
        /// 引数の要素と一致したのを削除
        /// クラスの場合だと==の定義が必要
        /// </summary>
        /// <param name="in_tData"></param>
        /// <returns></returns>
        const Bool Remove(const TYPE& in_rData)
        {
            Bool bRet = FALSE;
            // 要素が重複している可能性があるので全要素チェック
            for (Uint32 i = 0; i < this->_uSize; ++i)
            {
                if (this->_pBuff[i] == in_rData)
                {
                    this->_RemoveAt(i);
                    bRet = TRUE;
                }
            }

            return bRet;
        }

        TYPE& operator[](const Uint32 in_uIndex) const
        {
            HE_ASSERT(0 < this->_uSize);
            return this->_pBuff[in_uIndex];
        }

        void operator=(VectorBase& in_vrData) { this->_DeepCopy(in_vrData); }
        void operator=(const VectorBase& in_vrData) { this->_DeepCopy(in_vrData); }

        TYPE* GetPtr(const Uint32 in_uIndex) const
        {
            HE_ASSERT(0 < this->_uSize);
            return &this->_pBuff[in_uIndex];
        }

    protected:
        void _DeepCopy(const VectorBase& in_vrData)
        {
            if (in_vrData._uSize <= 0) return;

            const Uint32 uNewSize = HE_MIN(this->_uCapacity, in_vrData._uSize);
            std::copy(in_vrData._pBuff, in_vrData._pBuff + uNewSize, this->_pBuff);
            this->_uSize = uNewSize;
        }

    private:
        /// <summary>
        /// 指定インデックス削除
        /// Vectorはデータを挿入するとデータ位置が変わってしまう
        /// 使う側がインデックスを保存して保存したインデックスを使って削除すると消したくないデータが削除される可能性がある
        /// このようなエラーを出来るだけ起こさないようにできるだけインデックスを使わせない作りにしている
        /// </summary>
        void _RemoveAt(const Uint32 in_uIndex)
        {
            HE_ASSERT(in_uIndex < this->_uSize);
            HE_ASSERT(0 < this->_uSize);

            Uint32 uLastIndex = this->_uSize - 1;
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

            --this->_uSize;
        }

    protected:
        Uint32 _uSize = 0;

    private:
        TYPE* _pBuff      = NULL;
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
        CustomFixVector(CustomFixVector& in_rSrc) : VectorBase<TYPE>(this->_aBuff, CAPACITY)
        {
            this->_DeepCopy(in_rSrc);
        }

        CustomFixVector(const CustomFixVector& in_rSrc) : VectorBase<TYPE>(this->_aBuff, CAPACITY)
        {
            this->_DeepCopy(in_rSrc);
        }

        // コンストラクタ (initializer_listを受け取る)
        CustomFixVector(const std::initializer_list<TYPE>& in_rInitList)
            : VectorBase<TYPE>(this->_aBuff, CAPACITY)
        {
            if (in_rInitList.size() <= 0) return;

            const Uint32 uSize = HE_MIN(CAPACITY, in_rInitList.size());

            auto it = in_rInitList.begin();
            for (Uint32 i = 0; i < uSize; ++i)
            {
                this->_aBuff[i] = *it;
                ++it;
            }

            this->_uSize = uSize;
        }

        virtual ~CustomFixVector() = default;

    private:
        TYPE _aBuff[CAPACITY];
    };

    // テンプレートクラス CustomFixVector の部分的な型特性
    template <typename T>
    struct IsCustomFixVector : std::false_type
    {
    };

    // CustomFixVector のインスタンスに対する特殊化
    template <typename TYPE, Uint32 CAPACITY>
    struct IsCustomFixVector<CustomFixVector<TYPE, CAPACITY>> : std::true_type
    {
    };

}  // namespace Core::Common
