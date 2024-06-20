#pragma once

#include "Core/Core.h"

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
            void Empty() { this->_pBuff[0] = '\0'; }

            Sint32 Find(const Char* in_pStr, Uint32 in_start = 0) const;
            Bool IsEmpty() const { return this->_pBuff[0] == '\0'; }
            // データ個数
            inline Uint32 Size() const { return this->_size; }
            // 文字列の文字数
            // ワイド文字型だと配列の要素数=文字数とは限らないのでSizeメソッドとは別途文字数を取得するメソッドを用意
            const Uint32 Length() const;

            inline const Char* Str() const { return this->_pBuff; }

            /// <summary>
            /// UTF8として出力
            /// 文字列をUTF-8として利用したい場合に利用
            /// </summary>
            /// <param name="out"></param>
            /// <param name="in_size"></param>
            void OutputUTF8(Byte* out, Uint32 in_size)
            {
                E_ASSERT(out);
                E_ASSERT(in_size <= this->Size());
                // wchar_t型をutf8のcharに変えて出力
#ifdef _WIN
                ::memset(out, 0, sizeof(Byte) * in_size);
                Sint32 StrSize =
                    WideCharToMultiByte(CP_UTF8, 0, this->Str(), -1, NULL, 0, NULL, NULL);
                if (StrSize <= 0) return;

                StrSize = E_MIN(static_cast<Uint32>(StrSize), in_size);
                WideCharToMultiByte(CP_UTF8, 0, this->Str(), -1, out, StrSize, NULL, NULL);
#else
                // TODO: のちに対応
                E_ASSERT(0 && "Str()のをそのままコピーする");
#endif
            }

            // 大文字 / 小文字にする
            void ToLower() { E_STR_LOWER(this->_pBuff); }
            void ToUpper() { E_STR_UPPER(this->_pBuff); }

            FixStringBase& operator=(const Char* in_pStr)
            {
                this->_Copy(in_pStr, this->_size);
                return *this;
            }
            FixStringBase& operator=(const FixStringBase& r)
            {
                this->_Copy(r.Str(), this->_size);
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

        protected:
            FixStringBase(const FixStringBase& r) { this->_Copy(r.Str(), r.Length()); }

            FixStringBase& _Copy(const Char* in_pStr, const Uint32 in_strLen);

        private:
            void _Init(Char* pBuff, Uint32 buffsize);

            FixStringBase& _Add(const Char* in_pStr);
            FixStringBase& _Add(Char c);

        private:
            Char* _pBuff = NULL;
            Uint32 _size = 0;
        };

        /// <summary>
        /// 固定長の文字列クラス
        /// </summary>
        /// <typeparam name="SIZE"></typeparam>
        template <Uint32 SIZE>
        class FixString : public FixStringBase
        {
            E_CLASS_MOVE_CONSTRUCT_NG(FixString);

        public:
            FixString() : FixStringBase(_fixBuff, SIZE) {}
            // FixString(const Char* in_pStr) : FixStringBase(_fixBuff, SIZE) {
            // FixStringBase::operator=(in_pStr); }
            FixString(const Char* in_pStr) : FixStringBase(_fixBuff, SIZE)
            {
                this->_Copy(in_pStr, E_STR_LEN(in_pStr));
            }
            FixString(const FixString<SIZE>& r) : FixStringBase(_fixBuff, SIZE) { *this = r; }

#ifdef _WIN
            FixString(const Byte* in_pStr) : FixStringBase(_fixBuff, SIZE)
            {
                this->_ConvUTF8toWide(in_pStr, SIZE);
            }

            FixString(const std::string_view& in_rStr) : FixStringBase(_fixBuff, SIZE)
            {
                this->_ConvUTF8toWide(in_rStr.data(), static_cast<Uint32>(in_rStr.length()));
            }
#else
            FixString(const std::string_view& in_rStr) : FixStringBase(_fixBuff, SIZE)
            {
                // TODO: 対応が必要
                E_ASSERT(0);
            }
#endif

            FixString<SIZE>& operator=(const Char* in_pStr)
            {
                this->_Copy(in_pStr, E_STR_LEN(in_pStr));
                return *this;
            }
            FixString<SIZE>& operator=(const FixString<SIZE>& r)
            {
                this->_Copy(r._fixBuff, E_STR_LEN(r._fixBuff));
                return *this;
            }
            /*
            {
                E_STR_CPY_S(this->_fixBuff, SIZE, r._fixBuff, E_STR_LEN(r._fixBuff));
                return *this;
            }
            */

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
                E_ASSERT(in_len <= SIZE);

                static Char w[SIZE] = {};
                ::memset(w, 0, E_ARRAY_SIZE(w));

                // TODO: 利用する文字数を取得
                // lengthメソッドだとワイド文字の最大数がわからない
                // ワイド文字の後ろから改行位置を求めて削除とかできるかな
                // Sint32 sizeNeeded = static_cast<Sint32>(in_rStr.length());
                Sint32 sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, in_pStr, in_len, NULL, 0);
                // 利用する文字数が制限を超えていないかチェック
                E_ASSERT(sizeNeeded < SIZE);

                // UTF8文字列からUTF16の文字列に変える
                MultiByteToWideChar(CP_UTF8, 0, in_pStr, SIZE, &w[0], sizeNeeded);

                (*this) = w;

                return TRUE;
            }
#endif
        private:
            Char _fixBuff[SIZE] = {};
        };

        // 利用できる固定長の文字列型を用意
        typedef FixString<16> FixString16;
        typedef FixString<32> FixString32;
        typedef FixString<64> FixString64;
        typedef FixString<128> FixString128;
        typedef FixString<256> FixString256;
        typedef FixString<512> FixString512;
        typedef FixString<1024> FixString1024;
    }  // namespace Common
}  // namespace Core
