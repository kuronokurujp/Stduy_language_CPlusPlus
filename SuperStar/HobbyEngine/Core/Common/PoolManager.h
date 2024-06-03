#pragma once

#include "MiniEngine.h"
#include "Core/Common/Handle.h"
#include "Core/Common/FixArray.h"

#include <vector>
#include <list>

namespace Core
{
    namespace Common
    {
        /// <summary>
        /// プールしたデータを使いまわすためのクラス
        /// データを使いまわしてメモリ確保と解放の処理コストを減らせる
        /// プールデータ型をテンプレートで指定する
        /// 継承利用版
        /// </summary>
        /// <typeparam name="T"></typeparam>
        template <typename T>
        class BasePoolManager
        {
        public:
            /// <summary>
            /// 割り当てた結果データ
            /// </summary>
            struct AllocData
            {
                T* _pItem = NULL;
                Handle _handle;
            };

            /// <summary>
            /// プールのバッファ予約
            /// プールするためのデータバッファ数を指定して確保
            /// 利用前に必ず実行
            /// </summary>
            /// <param name="in_max"></param>
            void Reserve(const Uint32 in_max)
            {
                this->_cacheDatas.reserve(in_max);
            }

            /// <summary>
            /// データ使用個数
            /// </summary>
            /// <returns></returns>
            const Uint32 UseCount() const
            {
                return static_cast<Uint32>(this->_userSlot.size());
            }

            /// <summary>
            /// データ最大数
            /// </summary>
            /// <returns></returns>
            const Uint32 Max() const
            {
                return static_cast<Uint32>(this->_cacheDatas.capacity());
            }

            const bool Empty() const
            {
                return (this->UseCount() <= 0);
            }

            // 現在利用しているデータリストを取得
            const std::list<AllocData>& GetUserDataList() const { return this->_userSlot; }

        protected:
            static const Uint32 NON_MAGIC_NUMBER = 0;

            /// <summary>
            /// プールしているデータの中で利用できるデータ枠を取得
            /// 利用するデータとそのデータを紐づけたハンドルを返す
            /// </summary>
            /// <returns></returns>
            //std::tuple<T*, Handle> _Alloc()
            // Sのクラスは必ずTを継承したクラスでないといけない
            template<class S>
            AllocData _Alloc()
            {
                AllocData allocData;

                // 割り当てられなかったら空の枠を返す
                if (this->UseCount() >= this->_cacheDatas.capacity())
                    return allocData;

                Handle handle;

                // TODO: 失敗した場合に必ずエラーになるインデックス値を設定
                Bool bNewSlot = FALSE;
                S* object = NULL;

                if (this->_cacheDatas.empty())
                {
                    bNewSlot = TRUE;
                }
                else
                {
                    Bool bFreeSlot = FALSE;

                    // TODO: フリー領域にあるのがSクラスかどうかチェックしてあればそれを使う, なければ新規作成する
                    // すでに生成したTクラスのインスタンスを再利用
                    Uint32 chkIndex = 0;
                    for (auto b = this->_cacheDatas.begin(); b != this->_cacheDatas.end(); ++b)
                    {
                        // 再利用可能なデータかチェック
                        object = dynamic_cast<S*>(*b);
                        if (object != NULL)
                        {
                            // 再利用可能なデータなので追加フラグを立てる
                            bFreeSlot = TRUE;

                            // キャッシュデータを利用したのでキャッシュリストから外す
                            this->_cacheDatas.erase(b);
                            handle.Init(chkIndex);

                            break;
                        }

                        ++chkIndex;
                    }

                    if (bFreeSlot == FALSE)
                    {
                        bNewSlot = TRUE;
                    }
                }

                if (bNewSlot)
                {
                    ++this->_indexCount;
                    handle.Init(this->_indexCount);

                    // Tを継承したSクラスのインスタンスを生成
                    // NEWは用意したマクロを使う
                    object = new S();

                }

                allocData._handle = handle;
                allocData._pItem = object;

                // 利用リストに追加
                this->_userSlot.push_back(allocData);

                return allocData;
            } 

            /// <summary>
            /// 割り当てデータを解放
            /// </summary>
            /// <param name="handle"></param>
            void _Free(const Handle& in_handle, const Bool in_bCache)
            {
                if (in_handle.Null())
                    return;

                T* pRemoveObj = NULL;
                for (auto it = this->_userSlot.begin(); it != this->_userSlot.end(); ++it)
                {
                    if (it->_handle.Magic() == in_handle.Magic())
                    {
                        pRemoveObj = it->_pItem;
                        // 利用リストから外す
                        this->_userSlot.erase(it);

                        break;
                    }
                }
                E_ASSERT(pRemoveObj != NULL);

                if (in_bCache)
                {
                    // メモリ確保した要素をキャッシュして使いまわす
                    this->_cacheDatas.push_back(pRemoveObj);
                }
                else
                {
                    // キャッシュしたのを破棄する
                    E_SAFE_DELETE(pRemoveObj);
                }
            }

            // データの参照(非const版)
            T* _Ref(const Handle& in_handle)
            {
                if (in_handle.Null())
                    return NULL;

                for (auto it = this->_userSlot.begin(); it != this->_userSlot.end(); ++it)
                {
                    if (it->_handle.Magic() == in_handle.Magic())
                    {
                        return it->_pItem;
                    }
                }

                return NULL;
            }

            // データの参照(const版)
            const T* _Ref(const Handle& in_handle) const
            {
                typedef BasePoolManager<T> ThisType;
                return (const_cast<ThisType*>(this)->_Ref(in_handle));
            }

        private:
            // 利用中のデータスロット
            std::list<AllocData> _userSlot;

            // 再利用するキャッシュデータリスト 
            std::vector<T*> _cacheDatas;
            Uint32 _indexCount = 0;
        };

        /// <summary>
        /// プールしたデータを使いまわすためのクラス
        /// データを使いまわしてメモリ確保と解放の処理コストを減らせる
        /// プールデータ型をテンプレートで指定する
        /// データ最大数は固定
        /// </summary>
        /// <typeparam name="T"></typeparam>
        template <typename T, Uint32 SIZE>
        class FixPoolManager
        {
        private:
            static const Uint32 NON_MAGIC_NUMBER = 0;

        public:
            /// <summary>
            /// データ使用個数
            /// </summary>
            /// <returns></returns>
            const Uint32 UseCount() const
            {
                return static_cast<Uint32>(this->_userSlot.Size());
            }

            const Uint32 Max() const
            {
                return static_cast<Uint32>(this->_magicNumbers.Max());
            }
            
            const bool Empty() const
            {
                return (this->UseCount() <= 0);
            }

            // 現在利用しているデータリストを取得
            const std::list<T*>& GetUserDataList() const { return this->_userSlot; }

            /// <summary>
            /// プールしているデータの中で利用できるデータ枠を取得
            /// 利用するデータとそのデータを紐づけたハンドルを返す
            /// </summary>
            /// <returns></returns>
            T* Alloc(Handle* out_pHandle)
            {
                E_ASSERT(out_pHandle != NULL);

                Uint32 index = 0;
                if (this->_freeSlots.IsEmpty())
                {
                    this->_userSlot.PushBack(T());

                    index = this->_magicNumbers.Size();
                    out_pHandle->Init(index);
                    this->_magicNumbers.PushBack(out_pHandle->Magic());
                }
                else
                {
                    index = this->_freeSlots.Back();
                    out_pHandle->Init(index);

                    (*this->_magicNumbers.GetPtr(index)) = out_pHandle->Magic();
                }

                return this->_userSlot.GetPtr(index);
            } 

            /// <summary>
            /// 割り当てデータを解放
            /// </summary>
            /// <param name="handle"></param>
            void Free(const Handle& in_handle)
            {
                if (in_handle.Null())
                    return;

                const Uint32 index = in_handle.Index();
                E_ASSERT(this->_magicNumbers[index] != NON_MAGIC_NUMBER);
                E_ASSERT(this->_magicNumbers[index] == in_handle.Magic());
                E_ASSERT(index < this->_userSlot.Size());

                (*this->_magicNumbers.GetPtr(index)) = NON_MAGIC_NUMBER;
                this->_freeSlots.PushBack(index);
            }

            // データの参照(非const版)
            T* _Ref(const Handle& in_handle)
            {
                if (in_handle.Null())
                    return NULL;
                
                Uint32 index = in_handle.Index();
                if ((this->_magicNumbers[index] != in_handle.Magic()))
                    return NULL;

                return this->_userSlot[index];
            }

            // データの参照(const版)
            const T* _Ref(const Handle& in_handle) const
            {
                typedef BasePoolManager<T> ThisType;
                return (const_cast<ThisType*>(this)->_Ref(in_handle));
            }

        private:
            FastFixArray<T, SIZE> _userSlot;
            FastFixArray<Uint32, SIZE> _freeSlots;
            FastFixArray<Uint32, SIZE> _magicNumbers;
        };
    }
};

