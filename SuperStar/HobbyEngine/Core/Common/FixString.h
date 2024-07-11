#pragma once

#include <memory>

#include "Core/Core.h"
#include "FixArray.h"

#ifdef _WIN

#include <string_view>

#endif

namespace Core
{
    namespace Common
    {
        /// <summary>
        /// 文字列制御の基本クラス
        /// std::stringのようにクラス内部でメモリ確保する事はしない
        /// 固定長文字列のクラス生成を前提
        /// </summary>
        class FixStringBase
        {
            E_CLASS_DEFAULT_CONSTRUCT_NG(FixStringBase);
            E_CLASS_MOVE_CONSTRUCT_NG(FixStringBase);

        public:
            FixStringBase(Char* in_pBuff, Uint32 in_size);

            virtual ~FixStringBase() {}

            FixStringBase& Replace(const Char* in_pOld, const Char* in_pNew);
            FixStringBase& Insert(Uint32 in_index, const Char* in_pInsertStr);
            FixStringBase& Remove(Uint32 in_index, Uint32 in_count = 1);
            FixStringBase& Format(const Char* in_pFormat, ...);
            FixStringBase& FormatV(const Char* in_pFormat, va_list vlist);
            void Clear() E_NOEXCEPT { this->_pBuff[0] = '\0'; }

            Sint32 Find(const Char* in_pStr, Uint32 in_start = 0) const;

            Bool Empty() const E_NOEXCEPT { return this->_pBuff[0] == '\0'; }

            // 文字列の文字容量数
            inline Uint32 Capacity() const E_NOEXCEPT { return this->_capacity; }

            // 文字列の文字数
            // ワイド文字型だと配列の要素数=文字数とは限らないのでSizeメソッドとは別途文字数を取得するメソッドを用意
            const Uint32 Length() const;

            inline const Char* Str() const E_NOEXCEPT { return this->_pBuff; }

            /// <summary>
            /// 文字列をハッシュ化して返す
            /// </summary>
            /// <returns></returns>
            const Uint64 Hash();

            /// <summary>
            /// UTF8として出力
            /// 文字列をUTF-8として利用したい場合に利用
            /// </summary>
            /// <param name="out"></param>
            /// <param name="in_size"></param>
            void OutputUTF8(Byte* out, Uint32 in_size) const;

            // 大文字 / 小文字にする
            void ToLower() { E_STR_LOWER(this->_pBuff); }
            void ToUpper() { E_STR_UPPER(this->_pBuff); }

            FixStringBase& operator=(const Char* in_pStr)
            {
                this->_Copy(in_pStr, this->_capacity);
                return *this;
            }

            FixStringBase& operator=(const FixStringBase& r)
            {
                this->_Copy(r.Str(), this->_capacity);
                return *this;
            }

            FixStringBase& operator+=(Char c)
            {
                this->_Add(c);
                return *this;
            }
            FixStringBase& operator+=(const Char* in_pStr)
            {
                this->_Add(in_pStr);
                return *this;
            }
            FixStringBase& operator+=(const FixStringBase& r)
            {
                this->_Add(r.Str());
                return *this;
            }

            // いくつかの型を指定できるようにする
            template <typename... Args>
            typename std::enable_if<(std::is_same<Args, const Char*>::value && ...), void>::type
            Concatenate(Args... args)
            {
                // 引数の個数を取得
                Uint32 count = static_cast<Uint32>(sizeof...(args));
                if (count <= 0) return;

                // 初期化リストを使用して引数を処理
                const Char* values[] = {args...};
                for (Uint32 i = 0; i < count; ++i) this->_Add(values[i]);
            }

            Bool operator==(const Char* in_pStr) const;
            Bool operator==(const FixStringBase& in_rhs) const { return operator==(in_rhs.Str()); }
            Bool operator!=(const Char* in_pStr) const { return !operator==(in_pStr); }
            Bool operator!=(const FixStringBase& rhs) const { return !operator==(rhs); }
            Bool operator<(const Char* in_pStr) const
            {
                return E_STR_CMP(this->_pBuff, in_pStr) < 0;
            }
            Bool operator<(const FixStringBase& in_rhs) const
            {
                return E_STR_CMP(this->_pBuff, in_rhs.Str()) < 0;
            }
            Bool operator<=(const Char* in_pStr) const
            {
                return E_STR_CMP(this->_pBuff, in_pStr) <= 0;
            }
            Bool operator<=(const FixStringBase& in_rhs) const
            {
                return E_STR_CMP(this->_pBuff, in_rhs.Str()) <= 0;
            }
            Bool operator>(const Char* in_pStr) const
            {
                return E_STR_CMP(this->_pBuff, in_pStr) > 0;
            }
            Bool operator>(const FixStringBase& in_rhs) const
            {
                return E_STR_CMP(this->_pBuff, in_rhs.Str()) > 0;
            }
            Bool operator>=(const Char* in_pStr) const
            {
                return E_STR_CMP(this->_pBuff, in_pStr) >= 0;
            }
            Bool operator>=(const FixStringBase& in_rhs) const
            {
                return E_STR_CMP(this->_pBuff, in_rhs.Str()) >= 0;
            }

            const Char operator[](const Uint32 in_idx) const
            {
                if (this->_capacity <= in_idx)
                {
                    return '\0';
                }

                return this->_pBuff[in_idx];
            }

        protected:
            FixStringBase(const FixStringBase& r) { this->_Copy(r.Str(), r.Length()); }

            FixStringBase& _Copy(const Char* in_pStr, const Uint32 in_strLen);

        private:
            void _Init(Char* pBuff, Uint32 in_size);

            FixStringBase& _Add(const Char* in_pStr);
            FixStringBase& _Add(Char c);

        private:
            Char* _pBuff     = NULL;
            Uint32 _capacity = 0;
        };

        /// <summary>
        /// 固定長の文字列クラス
        /// </summary>
        /// <typeparam name="SIZE"></typeparam>
        template <Uint32 CAPACITY>
        class FixString : public FixStringBase
        {
            E_CLASS_MOVE_CONSTRUCT_NG(FixString);

        public:
            FixString() : FixStringBase(this->_fixBuff, CAPACITY) {}
            FixString(const Char* in_pStr) : FixStringBase(this->_fixBuff, CAPACITY)
            {
                this->_Copy(in_pStr, E_STR_LEN(in_pStr));
            }
            FixString(const FixString<CAPACITY>& r) : FixStringBase(this->_fixBuff, CAPACITY)
            {
                *this = r;
            }

#ifdef _WIN
            FixString(const Byte* in_pStr) : FixStringBase(this->_fixBuff, CAPACITY)
            {
                this->_ConvUTF8toWide(in_pStr, static_cast<Uint32>(::strlen(in_pStr)));
            }

            FixString(const std::string_view& in_rStr) : FixStringBase(this->_fixBuff, CAPACITY)
            {
                this->_ConvUTF8toWide(in_rStr.data(), static_cast<Uint32>(in_rStr.length()));
            }
#else
            FixString(const std::string_view& in_rStr) : FixStringBase(_fixBuff, CAPACITY)
            {
                // TODO: 対応が必要
                E_ASSERT(0);
            }
#endif

            FixString<CAPACITY>& operator=(const Char* in_pStr)
            {
                this->_Copy(in_pStr, E_STR_LEN(in_pStr));
                return *this;
            }

// win版では文字列はwchar / charの二つの型がある
#ifdef _WIN
            FixString<CAPACITY>& operator=(const Byte* in_pStr)
            {
                this->_ConvUTF8toWide(in_pStr, CAPACITY);
                return *this;
            }
#endif

            FixString<CAPACITY>& operator=(const FixString<CAPACITY>& r)
            {
                this->_Copy(r._fixBuff, E_STR_LEN(r._fixBuff));
                return *this;
            }

        private:
#ifdef _WIN
            /// <summary>
            /// UTF8の文字列からワイドの文字列に変えて設定
            /// </summary>
            /// <param name="in_pStr"></param>
            /// <param name="in_len"></param>
            /// <returns></returns>
            const Bool _ConvUTF8toWide(const Byte* in_pStr, const Uint32 in_len)
            {
                E_ASSERT(in_pStr);
                E_ASSERT(in_len <= CAPACITY);

                static Char w[CAPACITY] = {};
                ::memset(w, 0, E_ARRAY_SIZE(w));

                // 利用する文字数を取得
                Sint32 sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, in_pStr, in_len, NULL, 0);
                // 利用する文字数が制限を超えていないかチェック
                E_ASSERT(sizeNeeded < CAPACITY);

                // UTF8文字列からUTF16の文字列に変える
                MultiByteToWideChar(CP_UTF8, 0, in_pStr, CAPACITY, &w[0], sizeNeeded);

                (*this) = w;

                return TRUE;
            }
#endif
        private:
            Char _fixBuff[CAPACITY] = {};
        };

        // 固定長の文字列型
        typedef FixString<16> FixString16;
        typedef FixString<32> FixString32;
        typedef FixString<64> FixString64;
        typedef FixString<128> FixString128;
        typedef FixString<256> FixString256;
        typedef FixString<512> FixString512;
        typedef FixString<1024> FixString1024;

        /// <summary>
        /// 指定文字列を指定文字で区切って出力
        /// </summary>
        /// <param name="out"></param>
        /// <param name="in_rStr"></param>
        /// <param name="in_delim"></param>
        template <Uint32 SPLITE_COUNT>
        static void OutputSplitString(FastFixArray<FixString1024, SPLITE_COUNT>& out,
                                      FixStringBase& in_rStr, const Char in_delim)
        {
            FixString1024 item;
            Uint32 i = 0;
            Char ch  = in_rStr[i];
            while (ch != '\0')
            {
                if (ch == in_delim)
                {
                    if (item.Empty() == FALSE) out.PushBack(item);
                    item.Clear();
                    // TODO: 配列要素数 -
                    // 1が文字列分割できる回数なので回数を上回ったたら分割処理は終了
                }
                else
                {
                    item += ch;
                }
                ++i;
                ch = in_rStr[i];
            }

            if (item.Empty() == FALSE) out.PushBack(item);
        }

    }  // namespace Common
}  // namespace Core
