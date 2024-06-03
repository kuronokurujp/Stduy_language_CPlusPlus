#pragma once


#include "Core/Core.h"
// #include <string.h>

namespace Core
{
    namespace Common
    {
        /// <summary>
        /// 文字列制御の基本クラス
        /// </summary>
        class FixStringBase
        {
        public:
            FixStringBase(Char* in_pBuff, Uint32 in_size);

            virtual ~FixStringBase() {}

            FixStringBase& Replace(const Char* in_pOld, const Char* in_pNew);
            FixStringBase& Insert(Uint32 in_index, const Char* in_pInsertStr);
            FixStringBase& Remove(Uint32 in_index, Uint32 in_count=1);
            FixStringBase& Format(const Char* in_pFormat, ...);
            FixStringBase& FormatV(const Char* in_pFormat, va_list vlist);
            void Empty() { this->_pBuff[0] = '\0'; }

            Sint32 Find(const Char* in_pStr, Uint32 in_start=0) const;
            Bool IsEmpty() const { return this->_pBuff[0] == '\0'; }
            // 文字列データ個数
            inline Uint32 Size() const { return static_cast<Uint32>(E_STR_LEN(this->_pBuff)); }
            // 文字列の文字数
            Uint32 Length();

            inline const Char* Str() const { return this->_pBuff; }
            // TODO: 大文字 / 小文字にする
            void ToLower() { E_STR_LOWER(this->_pBuff); }
            void ToUpper() { E_STR_UPPER(this->_pBuff); }

            FixStringBase& operator= (const Char* in_pStr) { this->Copy(in_pStr); return *this; }
            FixStringBase& operator= (const FixStringBase &r) { this->Copy(r.Str()); return *this; }
            FixStringBase& operator+=(Char c) { this->Add(c); return *this; }
            FixStringBase& operator+=(const Char* in_pStr) { this->Add(in_pStr); return *this; }
            FixStringBase& operator+=(const FixStringBase &r) { this->Add(r.Str()); return *this; }
            Bool operator==(const Char* in_pStr) const;
            Bool operator==(const FixStringBase &in_rhs) const { return  operator==(in_rhs.Str()); }
            Bool operator!=(const Char* in_pStr) const { return !operator==(in_pStr); }
            Bool operator!=(const FixStringBase &rhs) const	{ return !operator==(rhs); }
            Bool operator< (const Char* in_pStr) const { return E_STR_CMP(this->_pBuff, in_pStr) < 0; }
            Bool operator< (const FixStringBase &in_rhs) const { return E_STR_CMP(this->_pBuff, in_rhs.Str()) < 0; }
            Bool operator<=(const Char* in_pStr) const { return E_STR_CMP(this->_pBuff, in_pStr) <= 0; }
            Bool operator<=(const FixStringBase &in_rhs) const { return E_STR_CMP(this->_pBuff, in_rhs.Str()) <= 0; }
            Bool operator> (const Char* in_pStr) const { return E_STR_CMP(this->_pBuff, in_pStr) > 0; }
            Bool operator> (const FixStringBase &in_rhs) const { return E_STR_CMP(this->_pBuff, in_rhs.Str()) > 0; }
            Bool operator>=(const Char* in_pStr) const { return E_STR_CMP(this->_pBuff, in_pStr) >= 0; }
            Bool operator>=(const FixStringBase &in_rhs) const { return E_STR_CMP(this->_pBuff, in_rhs.Str()) >= 0; }

        protected:
            FixStringBase();
            FixStringBase(const FixStringBase &r) { Copy(r.Str()); }
         
        private:
            void _Init(Char* pBuff, Uint32 buffsize);

            FixStringBase& Copy(const Char* in_pStr);
            FixStringBase& Add(const Char* in_pStr);
            FixStringBase& Add(Char c);
        
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
        public:
            FixString() : FixStringBase(_fixBuff, SIZE) {}
            FixString(const Char* in_pStr) : FixStringBase(_fixBuff, SIZE) { FixStringBase::operator=(in_pStr); }
            FixString(const FixString<SIZE> &r) : FixStringBase(_fixBuff, SIZE) { *this = r; }

#ifdef _WIN
            FixString(const char* in_pStr) : FixStringBase(_fixBuff, SIZE)
            {
                static Char w[SIZE] = {};

                // mbstowcsでビルドエラーが出た
                // 以下のサイトを参考にして対応
                // https://stackoverflow.com/questions/22450423/how-to-use-crt-secure-no-warnings
                std::mbstowcs(&w[0], in_pStr, SIZE);

                FixStringBase::operator=(w);
            }
#endif

            FixString<SIZE>& operator=(const FixString<SIZE> &r)
            {
                E_STR_CPY_S(this->_fixBuff, SIZE, r._fixBuff, E_STR_LEN(r._fixBuff));
                return *this;
            }
        
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

#ifdef _WIN
        /// <summary>
        /// Byte文字列をWide文字列に変えたのに出力
        /// </summary>
        /// <param name="in_pByte"></param>
        /// <param name="in_size"></param>
        /// <returns></returns>
        extern void ByteToWideChar(Char* out_wides,  const Byte in_bytes[], const Uint32 in_wideSize);
        extern Char* ByteToWideCharByTemp(const Byte in_bytes[]);
#endif
    }
}

