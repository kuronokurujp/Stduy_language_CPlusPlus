#pragma once

#include "Engine/Core.h"

namespace Core::Common
{
    /// <summary>
    /// インデックスをハンドルとして持つクラス
    /// </summary>
    class Handle final
    {
    public:
        Handle() { this->Clear(); }
        // 値をコピーする
        Handle(Handle& in_rHandle) { this->_ulHandle = in_rHandle._ulHandle; }
        Handle(const Handle& in_crHandle) { this->_ulHandle = in_crHandle._ulHandle; }

        // ムーブセマンティクス
        // コピーと同じにする
        Handle(Handle&& r) { this->_ulHandle = r._ulHandle; }

        /// <summary>
        /// 初期化
        /// 管理するindexを設定
        /// </summary>
        /// <param name="index"></param>
        void Init(const Uint32 in_uIndex)
        {
            HE_ASSERT(this->Null());
            HE_ASSERT(in_uIndex <= E_MAX_INDEX);

            // マジックナンバーを割り当てる数を作成
            // TODO: GUIDの方がいいのだろうか？
            // 24時間で1年中稼働するシステムなら対応が必要
            static Uint32 s_magicNumber = 0;
            ++s_magicNumber;
            // マジックナンバーが枯渇しているかチェック
            HE_ASSERT(s_magicNumber < E_MAX_MAGIC);

            // インデックスとマジックナンバーを割り当て
            this->_handleField._index = in_uIndex;
            this->_handleField._magic = s_magicNumber;
        }

        void Clear() HE_NOEXCEPT { this->_ulHandle = 0; }

        inline const Uint32 Index() const HE_NOEXCEPT { return this->_handleField._index; }
        inline const Uint32 Magic() const HE_NOEXCEPT { return this->_handleField._magic; }
        inline const Bool Null() const HE_NOEXCEPT { return (this->_ulHandle == 0); }

        operator const Uint64() const HE_NOEXCEPT { return this->_ulHandle; }

        inline const Bool operator!=(Handle& r) HE_NOEXCEPT
        {
            return (this->_ulHandle != r._ulHandle);
        }

        inline const Bool operator==(Handle& r) HE_NOEXCEPT
        {
            return (this->_ulHandle == r._ulHandle);
        }

        void operator=(const Handle& r) HE_NOEXCEPT { this->_ulHandle = r._ulHandle; }

        void operator=(Handle&& r) HE_NOEXCEPT { this->_ulHandle = r._ulHandle; }

    private:
        enum
        {
            // インデックスとマジックナンバーの使用ビットフィールドサイズ
            // 今のところ合計64bitまで使用可能
            E_SIZE_INDEX_BIT = 32,
            E_SIZE_MAGIC_BIT = 32,

            // インデックスとマジックナンバーの最大値
            // ハンドルを扱うことが出来る最大数
            E_MAX_INDEX = (1LL << E_SIZE_INDEX_BIT) - 1,
            E_MAX_MAGIC = (1LL << E_SIZE_MAGIC_BIT) - 1,
        };

        /// <summary>
        /// ハンドルするデータ
        /// </summary>
        union
        {
            struct
            {
                // インデックス
                Uint32 _index : E_SIZE_INDEX_BIT;
                // マジックナンバー
                Uint32 _magic : E_SIZE_MAGIC_BIT;
            } _handleField;

            Uint64 _ulHandle = 0;
        };
    };

}  // namespace Core::Common

// Handleクラスをunordered_mapで利用できるようにするための対応
// 詳細は以下
// https://qiita.com/izmktr/items/8e0fd1b6e37de59a9bd0
namespace std
{
    template <>
    class hash<Core::Common::Handle>
    {
    public:
        size_t operator()(const Core::Common::Handle& p) const { return p.operator const Uint64(); }
    };
}  // namespace std

// 空ハンドル
// ハンドルを戻り値とする処理で失敗した時の戻り値に利用
extern const Core::Common::Handle InvalidHandle;
