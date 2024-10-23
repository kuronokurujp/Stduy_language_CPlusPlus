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
        Handle(Handle&& r) HE_NOEXCEPT { this->_ulHandle = r._ulHandle; }

        /// <summary>
        /// 初期化
        /// 管理するindexを設定
        /// </summary>
        void Init(const Uint32 in_uIndex);

        void Clear() HE_NOEXCEPT { this->_ulHandle = 0; }

        Uint32 Index() const HE_NOEXCEPT { return this->_handleField._uIndex; }
        Uint32 Magic() const HE_NOEXCEPT { return this->_handleField._uMagic; }
        Bool Null() const HE_NOEXCEPT { return (this->_ulHandle == 0); }

        operator Uint64() const HE_NOEXCEPT { return this->_ulHandle; }

        Bool operator!=(Handle& r) HE_NOEXCEPT { return (this->_ulHandle != r._ulHandle); }

        Bool operator==(Handle& r) HE_NOEXCEPT { return (this->_ulHandle == r._ulHandle); }

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
                Uint32 _uIndex : E_SIZE_INDEX_BIT;
                // マジックナンバー
                Uint32 _uMagic : E_SIZE_MAGIC_BIT;
            } _handleField;

            Uint64 _ulHandle = 0;
        };
    };

}  // namespace Core::Common

// TODO: 使わないか確認
// Handleクラスをunordered_mapで利用できるようにするための対応
// 詳細は以下
// https://qiita.com/izmktr/items/8e0fd1b6e37de59a9bd0
namespace std
{
    template <>
    class hash<Core::Common::Handle>
    {
    public:
        size_t operator()(const Core::Common::Handle& p) const { return p.operator Uint64(); }
    };
}  // namespace std

// 空ハンドル
// ハンドルを戻り値とする処理で失敗した時の戻り値に利用
extern const Core::Common::Handle NullHandle;
