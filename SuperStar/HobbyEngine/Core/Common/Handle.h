#pragma once

#include "MiniEngine.h"

namespace Core
{
    namespace Common
    {
        /// <summary>
        /// インデックスをハンドルとして持つクラス
        /// </summary>
        class Handle
        {
        public:
            Handle() { this->Clear(); }
            // 値をコピーする
            Handle(Handle& in_rHandle) { this->_handle = in_rHandle._handle; }
            Handle(const Handle& in_rHandle) { this->_handle = in_rHandle._handle; }

            // ムーブセマンティクス
            // コピーと同じにする
            Handle(Handle&& r) { this->_handle = r._handle; }

            /// <summary>
            /// 初期化
            /// 管理するindexを設定
            /// </summary>
            /// <param name="index"></param>
            void Init(const Uint32 in_index)
            {
                E_ASSERT(this->Null());
                E_ASSERT(in_index <= MAX_INDEX);
                
                // マジックナンバーを割り当てる数を作成
                // TODO: GUIDの方がいいのだろうか？
                // 24時間で1年中稼働するシステムなら対応が必要
                static Uint32 s_magicNumber = 0;
                ++s_magicNumber;
                // マジックナンバーが枯渇しているかチェック
                E_ASSERT(s_magicNumber < MAX_MAGIC);

                // インデックスとマジックナンバーを割り当て
                this->_handleField._index = in_index;
                this->_handleField._magic = s_magicNumber;
            }

            void Clear()
            {
                this->_handle = 0;
            }

            inline const Uint32 Index() const { return this->_handleField._index; }
            inline const Uint32	Magic() const { return this->_handleField._magic; }
            inline const Bool Null() const { return (this->_handle == 0); }

            operator const Uint64 () const { return this->_handle; }

            // ハンドルオペレーター
            inline const Bool operator!=(Handle& r)
            {
                return (this->_handle != r._handle);
            }

            inline const Bool operator==(Handle& r)
            {
                return (this->_handle == r._handle);
            }

            void operator=(const Handle& r)
            {
                this->_handle = r._handle;
            }

            void operator=(Handle && r)
            {
                this->_handle = r._handle;
            }

        private:
            enum
            {
                // インデックスとマジックナンバーの使用ビットフィールドサイズ
                // 今のところ合計64bitまで使用可能
                SIZE_INDEX_BIT = 32,
                SIZE_MAGIC_BIT = 32,

                // インデックスとマジックナンバーの最大値
                // ハンドルを扱うことが出来る最大数
                MAX_INDEX = (1LL << SIZE_INDEX_BIT) - 1,
                MAX_MAGIC = (1LL << SIZE_MAGIC_BIT) - 1,
            };

            /// <summary>
            /// ハンドルするデータ
            /// </summary>
            union
            {
                struct
                {
                    // インデックス
                    Uint32 _index:SIZE_INDEX_BIT;
                    // マジックナンバー
                    Uint32 _magic:SIZE_MAGIC_BIT;
                } _handleField;

                Uint64 _handle = 0;
            };
        };
    }
}

// 空ハンドル
// ハンドルを戻り値とする処理で失敗した時の戻り値に利用
static const Core::Common::Handle s_emptyHandle;
#define E_EMPTY_HANDLE s_emptyHandle;

