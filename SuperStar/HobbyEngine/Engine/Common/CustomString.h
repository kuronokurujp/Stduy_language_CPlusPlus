﻿#pragma once

#include <memory>

#include "CustomArray.h"
#include "Engine/Core.h"

#ifdef HE_WIN

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
            HE_CLASS_DEFAULT_CONSTRUCT_NG(FixStringBase);
            HE_CLASS_MOVE_CONSTRUCT_NG(FixStringBase);

        public:
            FixStringBase(Char* in_cpBuff, Uint32 in_uSize);

            virtual ~FixStringBase() {}

            FixStringBase& Replace(const Char* in_szOld, const Char* in_szNew);
            FixStringBase& Insert(Uint32 in_uIndex, const Char* in_szInsert);
            FixStringBase& Remove(Uint32 in_uIndex, Uint32 in_uCount = 1);
            FixStringBase& Format(const Char* in_cszFormat, ...);
            FixStringBase& FormatV(const Char* in_szFormat, va_list in_vlist);
            void Clear() HE_NOEXCEPT { this->_cpBuff[0] = '\0'; }

            Sint32 Find(const Char* in_szName, Uint32 in_uStart = 0) const;

            Bool Empty() const HE_NOEXCEPT { return this->_cpBuff[0] == '\0'; }

            // 文字列の文字容量数
            inline Uint32 Capacity() const HE_NOEXCEPT { return this->_uCapacity; }

            // 文字列の文字数
            // ワイド文字型だと配列の要素数=文字数とは限らないのでSizeメソッドとは別途文字数を取得するメソッドを用意
            const Uint32 Length() const;

            inline const Char* Str() const HE_NOEXCEPT { return this->_cpBuff; }

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
            void OutputUTF8(UTF8* out, const Uint32 in_uSize) const;

            // 大文字 / 小文字にする
            void ToLower() { HE_STR_LOWER(this->_cpBuff); }
            void ToUpper() { HE_STR_UPPER(this->_cpBuff); }

            FixStringBase& operator=(const Char* in_szName)
            {
                this->_Copy(in_szName, this->_uCapacity);
                return *this;
            }

            FixStringBase& operator=(const FixStringBase& r)
            {
                this->_Copy(r.Str(), this->_uCapacity);
                return *this;
            }

            FixStringBase& operator+=(Char c)
            {
                this->_Add(c);
                return *this;
            }
            FixStringBase& operator+=(const Char* in_szName)
            {
                this->_Add(in_szName);
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
                Uint32 uCount = static_cast<Uint32>(sizeof...(args));
                if (uCount <= 0) return;

                // 初期化リストを使用して引数を処理
                const Char* carrValue[] = {args...};
                for (Uint32 i = 0; i < uCount; ++i) this->_Add(carrValue[i]);
            }

            Bool operator==(const Char* in_szName) const
            {
                return (in_szName && (HE_STR_CMP(this->_cpBuff, in_szName) == 0));
            }

            Bool operator==(const FixStringBase& in_szrName) const
            {
                return operator==(in_szrName.Str());
            }
            Bool operator!=(const Char* in_szName) const { return !operator==(in_szName); }
            Bool operator!=(const FixStringBase& in_szrName) const
            {
                return !operator==(in_szrName);
            }
            Bool operator<(const Char* in_szName) const
            {
                return HE_STR_CMP(this->_cpBuff, in_szName) < 0;
            }
            Bool operator<(const FixStringBase& in_szrName) const
            {
                return HE_STR_CMP(this->_cpBuff, in_szrName.Str()) < 0;
            }
            Bool operator<=(const Char* in_szName) const
            {
                return HE_STR_CMP(this->_cpBuff, in_szName) <= 0;
            }
            Bool operator<=(const FixStringBase& in_szrName) const
            {
                return HE_STR_CMP(this->_cpBuff, in_szrName.Str()) <= 0;
            }
            Bool operator>(const Char* in_szName) const
            {
                return HE_STR_CMP(this->_cpBuff, in_szName) > 0;
            }
            Bool operator>(const FixStringBase& in_szrName) const
            {
                return HE_STR_CMP(this->_cpBuff, in_szrName.Str()) > 0;
            }
            Bool operator>=(const Char* in_szName) const
            {
                return HE_STR_CMP(this->_cpBuff, in_szName) >= 0;
            }
            Bool operator>=(const FixStringBase& in_szrName) const
            {
                return HE_STR_CMP(this->_cpBuff, in_szrName.Str()) >= 0;
            }

            const Char operator[](const Uint32 in_uCuIndex) const
            {
                if (this->_uCapacity <= in_uCuIndex)
                {
                    return '\0';
                }

                return this->_cpBuff[in_uCuIndex];
            }

        protected:
            FixStringBase(const FixStringBase& in_szrName)
            {
                this->_Copy(in_szrName.Str(), in_szrName.Length());
            }

            FixStringBase& _Copy(const Char* in_szName, const Uint32 in_uLen);

        private:
            void _Init(Char* in_cpBuff, Uint32 in_uSize);

            FixStringBase& _Add(const Char* in_szName);
            FixStringBase& _Add(Char c);

        private:
            Char* _cpBuff     = NULL;
            Uint32 _uCapacity = 0;
        };

        /// <summary>
        /// 固定長の文字列クラス
        /// </summary>
        /// <typeparam name="SIZE"></typeparam>
        template <Uint32 CAPACITY>
        class FixString : public FixStringBase
        {
            HE_CLASS_MOVE_CONSTRUCT_NG(FixString);

        public:
            FixString() : FixStringBase(this->_caBuff, CAPACITY) {}
            FixString(const Char* in_szName) : FixStringBase(this->_caBuff, CAPACITY)
            {
                this->_Copy(in_szName, HE_STR_LEN(in_szName));
            }
            FixString(const FixString<CAPACITY>& r) : FixStringBase(this->_caBuff, CAPACITY)
            {
                *this = r;
            }

#ifdef HE_WIN
            FixString(const UTF8* in_szNameUTF8) : FixStringBase(this->_caBuff, CAPACITY)
            {
                this->_ConvUTF8toWide(in_szNameUTF8, static_cast<Uint32>(::strlen(in_szNameUTF8)));
            }

            FixString(const std::string_view& in_szrName) : FixStringBase(this->_caBuff, CAPACITY)
            {
                this->_ConvUTF8toWide(in_szrName.data(), static_cast<Uint32>(in_szrName.length()));
            }
#else
            FixString(const std::string_view& in_rStr) : FixStringBase(this->_caBuff, CAPACITY)
            {
                // TODO: 対応が必要
                HE_ASSERT(0);
            }
#endif

            FixString<CAPACITY>& operator=(const Char* in_szName)
            {
                this->_Copy(in_szName, HE_STR_LEN(in_szName));
                return *this;
            }

// win版では文字列はwchar / charの二つの型がある
#ifdef HE_WIN
            FixString<CAPACITY>& operator=(const Sint8* in_szName)
            {
                this->_ConvUTF8toWide(in_szName, CAPACITY);
                return *this;
            }
#endif

            FixString<CAPACITY>& operator=(const FixString<CAPACITY>& r)
            {
                this->_Copy(r._caBuff, HE_STR_LEN(r._caBuff));
                return *this;
            }

        private:
#ifdef HE_WIN
            /// <summary>
            /// UTF8の文字列からワイドの文字列に変えて設定
            /// </summary>
            /// <param name="in_pStr"></param>
            /// <param name="in_len"></param>
            const Bool _ConvUTF8toWide(const UTF8* in_szNameUTF8, const Uint32 in_uLen)
            {
                HE_ASSERT(in_szNameUTF8);
                HE_ASSERT(in_uLen <= CAPACITY);

                static Char w[CAPACITY] = {};
                ::memset(w, 0, HE_ARRAY_SIZE(w));

                // 利用する文字数を取得
                Sint32 iUseSize = MultiByteToWideChar(CP_UTF8, 0, in_szNameUTF8, in_uLen, NULL, 0);
                // 利用する文字数が制限を超えていないかチェック
                HE_ASSERT(iUseSize < CAPACITY);

                // UTF8文字列からUTF16の文字列に変える
                MultiByteToWideChar(CP_UTF8, 0, in_szNameUTF8, CAPACITY, &w[0], iUseSize);

                (*this) = w;

                return TRUE;
            }
#endif
        private:
            Char _caBuff[CAPACITY] = {};
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
                                      FixStringBase& in_szrName, const Char in_cDelim)
        {
            FixString1024 name;
            Uint32 i = 0;
            Char c   = in_szrName[i];
            while (c != '\0')
            {
                if (c == in_cDelim)
                {
                    if (name.Empty() == FALSE) out.PushBack(name);
                    name.Clear();
                    // TODO: 配列要素数 -
                    // 1が文字列分割できる回数なので回数を上回ったたら分割処理は終了
                }
                else
                {
                    name += c;
                }
                ++i;
                c = in_szrName[i];
            }

            if (name.Empty() == FALSE) out.PushBack(name);
        }

    }  // namespace Common
}  // namespace Core
