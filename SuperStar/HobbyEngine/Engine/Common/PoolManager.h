﻿#pragma once

// 要素の数が動的に変わるので以下のデータ構造クラスを使っている
// TODO: 自前で動的対応したのを作るのでそれまでの利用
#include <list>
#include <unordered_map>
#include <vector>

#include "Engine/Common/CustomArray.h"
#include "Engine/Common/CustomStack.h"
#include "Engine/Common/CustomVector.h"
#include "Engine/Common/Handle.h"
#include "Engine/Core.h"
#include "Engine/Memory/Memory.h"

namespace Core::Common
{
    /// <summary>
    /// プールしたデータを使いまわすためのクラス
    /// データを使いまわしてメモリ確保と解放の処理コストを減らせる
    /// プールデータ型をテンプレートで指定する
    /// データを使いまわすのでクラス型を利用するとクラスのプロパティ値が残るので注意
    /// 継承利用版
    /// </summary>
    template <typename T>
    class RuntimePoolManager
    {
    public:
        struct AllocData
        {
            T* _pItem = NULL;
            Core::Common::Handle _handle;
        };

    public:
        virtual ~RuntimePoolManager() { this->_Release(); }

        /// <summary>
        /// データ使用個数
        /// </summary>
        /// <returns></returns>
        const Uint32 UseCount() const
        {
            if (this->_spUserSlot == NULL) return 0;

            return static_cast<Uint32>(this->_spUserSlot->size());
        }

        /// <summary>
        /// データ最大数
        /// </summary>
        /// <returns></returns>
        const Uint32 Max() const { return static_cast<Uint32>(this->_spCacheDatas->capacity()); }

        /// <summary>
        /// キャッシュしたデータが現在いくつか
        /// </summary>
        /// <returns></returns>
        const Uint32 CacheCount() const { return static_cast<Uint32>(this->_spCacheDatas->size()); }

        const bool Empty() const { return (this->UseCount() <= 0); }

        // 現在利用しているデータリストを取得
        // 参照にするとデータをコピーして遅くなるので注意
        inline const std::unordered_map<Core::Common::Handle, T*>* GetUserDataList() const
        {
            HE_ASSERT(this->_spUserSlot);
            return this->_spUserSlot.get();
        }

        /// <summary>
        /// ハンドルが存在するか
        /// </summary>
        Bool Valid(const Core::Common::Handle& in_h)
        {
            return (this->_spUserSlot->at(in_h) != NULL);
        }

    protected:
        static const Uint32 NON_MAGIC_NUMBER = 0;

        /// <summary>
        /// プールのバッファ予約
        /// プールするためのデータバッファ数を指定して確保
        /// 継承したクラスが必ず実行
        /// </summary>
        void _Reserve(const Uint32 in_uMax)
        {
            if (this->_spCacheDatas == NULL)
                this->_spCacheDatas = std::make_unique<std::vector<T*>>();

            if (this->_spUserSlot == NULL)
            {
                this->_spUserSlot =
                    std::make_unique<std::unordered_map<Core::Common::Handle, T*>>();
            }

            this->_spCacheDatas->reserve(in_uMax);
        }

        void _Release()
        {
            if (this->_spCacheDatas) this->_spCacheDatas.release();

            if (this->_spUserSlot) this->_spUserSlot.release();
        }

        /// <summary>
        /// プールしているデータの中で利用できるデータ枠を取得
        /// 利用するデータとそのデータを紐づけたハンドルを返す
        /// </summary>
        /// <returns></returns>
        template <class S>
        AllocData _Alloc()
        {
            static_assert(std::is_base_of<T, S>::value, "SクラスはTクラスを継承していない");

            HE_ASSERT(this->_spCacheDatas);
            HE_ASSERT(this->_spUserSlot);

            AllocData allocData;
            HE_ASSERT(0 < this->_spCacheDatas->capacity());

            // 割り当てられなかったら空の枠を返す
            if (this->UseCount() >= this->_spCacheDatas->capacity())
            {
                HE_ASSERT(FALSE && "オブジェクトを割り当てる数が足りない");
                return allocData;
            }

            Handle handle;

            Bool bNewSlot = FALSE;
            S* pObject    = NULL;

            if (this->_spCacheDatas->empty())
            {
                bNewSlot = TRUE;
            }
            else
            {
                Bool bFreeSlot = FALSE;

                // フリー領域にあるのがSクラスかどうかチェックしてあればそれを使う,
                // なければ新規作成する すでに生成したTクラスのインスタンスを再利用
                Uint32 uChkIndex = 0;
                for (auto b = this->_spCacheDatas->begin(); b != this->_spCacheDatas->end(); ++b)
                {
                    // 再利用可能なデータかチェック
                    pObject = dynamic_cast<S*>(*b);
                    if (pObject != NULL)
                    {
                        // 再利用可能なデータなので追加フラグを立てる
                        bFreeSlot = TRUE;

                        // キャッシュデータを利用したのでキャッシュリストから外す
                        this->_spCacheDatas->erase(b);
                        handle.Init(uChkIndex);

                        break;
                    }

                    ++uChkIndex;
                }

                if (bFreeSlot == FALSE)
                {
                    bNewSlot = TRUE;
                }
            }

            if (bNewSlot)
            {
                ++this->_uIndexCount;
                handle.Init(this->_uIndexCount);

                // Tを継承したSクラスのインスタンスを生成
                // NEWは用意したマクロを使う
                pObject = HE_NEW(S, 0);
            }

            allocData._handle = handle;
            allocData._pItem  = pObject;

            // 利用リストに追加
            this->_spUserSlot->insert(std::make_pair(handle, pObject));

            return allocData;
        }

        /// <summary>
        /// 割り当てデータを解放
        /// </summary>
        /// <param name="handle"></param>
        void _Free(const Handle& in_rHandle, const Bool in_bCache)
        {
            HE_ASSERT(in_rHandle.Null() == FALSE && "解放するデータがないとだめ");

            T* tpRemoveObj = this->_spUserSlot->at(in_rHandle);
            HE_ASSERT(tpRemoveObj != NULL);
            this->_spUserSlot->erase(in_rHandle);

            if (in_bCache)
            {
                // メモリ確保した要素をキャッシュして使いまわす
                this->_spCacheDatas->push_back(tpRemoveObj);
            }
            else
            {
                // キャッシュしたのを破棄する
                HE_SAFE_DELETE(tpRemoveObj);
            }
        }

        // データの参照(非const版)
        T* _Ref(const Handle& in_rHandle)
        {
            if (in_rHandle.Null()) return NULL;
            // 要素があるかチェック
            if (this->_spUserSlot->find(in_rHandle) != this->_spUserSlot->end())
                return this->_spUserSlot->at(in_rHandle);

            return NULL;
        }

        // データの参照(const版)
        const T* _Ref(const Handle& in_rHandle) const
        {
            typedef RuntimePoolManager<T> ThisType;
            return (const_cast<ThisType*>(this)->_Ref(in_rHandle));
        }

    private:
        // 利用中のデータスロット
        std::unique_ptr<std::unordered_map<Core::Common::Handle, T*>> _spUserSlot = NULL;

        // 再利用するキャッシュデータリスト
        std::unique_ptr<std::vector<T*>> _spCacheDatas = NULL;
        Uint32 _uIndexCount                            = 0;
    };

    /// <summary>
    /// プールしたデータを使いまわすためのクラス
    /// データを使いまわしてメモリ確保と解放の処理コストを減らせる
    /// プールデータ型をテンプレートで指定する
    /// データ最大数は固定
    /// データを使いまわすのでクラス型を利用するとクラスのプロパティ値が残るので注意
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template <typename T, Uint32 CAPACITY>
    class FixPoolManager final
    {
    private:
        static const Uint32 NON_MAGIC_NUMBER = 0;

    public:
        /// <summary>
        /// データ使用個数
        /// </summary>
        inline Uint32 Size() const HE_NOEXCEPT { return this->_uUseCount; }

        /// <summary>
        /// オブジェクトが空かどうか
        /// </summary>
        inline Bool Empty() const HE_NOEXCEPT { return (this->_uUseCount <= 0); }

        /// <summary>
        /// プールしているデータの中で利用できるデータ枠を取得
        /// 利用するデータとそのデータを紐づけたハンドルを返す
        /// </summary>
        T* Alloc(Handle* out)
        {
            HE_ASSERT(out != NULL);

            Uint32 uIndex = 0;
            if (this->_sFreeSlot.Empty())
            {
                uIndex = this->_vMagicNum.Size();
                out->Init(uIndex);
                this->_vMagicNum.PushBack(out->Magic());
            }
            else
            {
                uIndex = this->_sFreeSlot.PopBack();
                out->Init(uIndex);

                (*this->_vMagicNum.GetPtr(uIndex)) = out->Magic();
            }

            HE_ASSERT(uIndex < this->_vUserSlot.Capacity() &&
                      "プールオブジェクトのフリーインデックス値が確保数を超えている");

            ++this->_uUseCount;
            // 空きのあるスロットを使用する
            return &this->_vUserSlot[uIndex];
        }

        /// <summary>
        /// 割り当てデータを解放
        /// </summary>
        bool Free(const Handle& in_rHandle)
        {
            --this->_uUseCount;
            if (in_rHandle.Null()) return FALSE;

            const Uint32 uIndex = in_rHandle.Index();
            HE_ASSERT(this->_vMagicNum[uIndex] != NON_MAGIC_NUMBER);
            HE_ASSERT(this->_vMagicNum[uIndex] == in_rHandle.Magic());

            (*this->_vMagicNum.GetPtr(uIndex)) = NON_MAGIC_NUMBER;
            this->_sFreeSlot.PushBack(uIndex);

            return TRUE;
        }

        // データの参照(非const版)
        T* Ref(const Handle& in_rHandle)
        {
            if (in_rHandle.Null()) return NULL;

            Uint32 uIndex = in_rHandle.Index();
            // マジックナンバーが空値ならハンドルは解放済み
            if ((this->_vMagicNum[uIndex] == NON_MAGIC_NUMBER)) return NULL;
            if ((this->_vMagicNum[uIndex] != in_rHandle.Magic())) return NULL;

            return &this->_vUserSlot[uIndex];
        }

        // データの参照(const版)
        const T* Ref(const Handle& in_rHandle) const
        {
            typedef RuntimePoolManager<T> ThisType;
            return (const_cast<ThisType*>(this)->_Ref(in_rHandle));
        }

    private:
        CustomArray<T, CAPACITY> _vUserSlot;
        CustomFixStack<Uint32, CAPACITY> _sFreeSlot;
        CustomFixVector<Uint32, CAPACITY> _vMagicNum;

        Uint32 _uUseCount = 0;
    };
}  // namespace Core::Common
;  // namespace Core
