﻿#include "CustomString.h"

#include <stdarg.h>
#include <stdio.h>

#ifdef HE_WIN

#include <windows.h>

#endif

namespace Core
{
    namespace Common
    {
        FixStringBase::FixStringBase(Char* in_cpBuff, Uint32 in_uSize)
        {
            this->_Init(in_cpBuff, in_uSize);
        }

        void FixStringBase::_Init(Char* in_cpBuff, Uint32 in_uSize)
        {
            HE_ASSERT(in_cpBuff && in_uSize);

            this->_cpBuff    = in_cpBuff;
            this->_uCapacity = in_uSize;
            this->_cpBuff[0] = HE_STR_TEXT('\0');
        }

        FixStringBase& FixStringBase::Replace(const Char* in_cszOld, const Char* in_cszNew)
        {
            if (in_cszOld == NULL) return *this;

            if (in_cszNew == NULL) return *this;

            Uint32 uOldLength = static_cast<Uint32>(HE_STR_LEN(in_cszOld));
            Ptr pNewLength    = static_cast<Ptr>(HE_STR_LEN(in_cszNew));
            const Char* szSrc = this->_cpBuff;

            // 対象を探す
            do
            {
                const Char* szFind = HE_STR_STR(szSrc, in_cszOld);
                if (szFind == NULL) break;

                // 見つかった場所を抜いて新しい文字列に差し替え
                Ptr pAddr = szFind - this->_cpBuff;
                Remove(static_cast<Uint32>(pAddr), uOldLength);
                Insert(static_cast<Uint32>(pAddr), in_cszNew);
                szSrc = this->_cpBuff + pAddr + pNewLength;

                // もうこれ以上置き換えられない
                if (static_cast<Uint32>(szSrc - this->_cpBuff) >= this->_uCapacity) break;

            } while (*szSrc != HE_STR_TEXT('\0'));

            return *this;
        }

        FixStringBase& FixStringBase::Insert(Uint32 in_uIndex, const Char* in_szInsert)
        {
            Char* szBuffEnd   = this->_cpBuff + this->_uCapacity - 1;
            Uint32 uOriginLen = this->Length();
            Uint32 uInsertLen = ((!in_szInsert) ? 1 : static_cast<Uint32>(HE_STR_LEN(in_szInsert)));

            // 後ろに追加して終わり
            if (in_uIndex >= uOriginLen) return (*this) += in_szInsert;

            // 挿入サイズをチェックする
            if (in_uIndex + uInsertLen >= this->_uCapacity - 1)
                uInsertLen = this->_uCapacity - in_uIndex - 1;

            // 挿入箇所以降の文字を後ろへスライド
            {
                const Char* cszSrcTopAddr = this->_cpBuff + in_uIndex;
                Char* szSrcTail           = this->_cpBuff + uOriginLen;
                Char* szDst               = this->_cpBuff + uOriginLen + uInsertLen;

                // 溢れチェック
                if (szDst > szBuffEnd)
                {
                    Ptr cutLen = szDst - szBuffEnd;

                    szDst -= cutLen;
                    szSrcTail -= cutLen;
                    *szSrcTail = HE_STR_TEXT('\0');
                }
                HE_ASSERT(*szSrcTail == HE_STR_TEXT('\0'));

                // コピーする
                for (const Char* szSrc = szSrcTail; szSrc >= cszSrcTopAddr; --szSrc, --szDst)
                    *szDst = *szSrc;
            }

            // 挿入処理
            {
                Char* szDst       = this->_cpBuff + in_uIndex;
                const Char* szSrc = in_szInsert;

                if (in_szInsert)
                {
                    for (Uint32 i = 0; i < uInsertLen; ++i) *szDst++ = *szSrc++;
                }
                else
                {
                    *szDst++ = HE_STR_TEXT('\0');
                }
            }

            return *this;
        }

        FixStringBase& FixStringBase::Remove(Uint32 in_uIndex, Uint32 in_uCount)
        {
            Uint32 uSize = this->Capacity();
            Char* szDst  = this->_cpBuff + ((in_uIndex > uSize) ? uSize : in_uIndex);
            const Char* szSrc =
                this->_cpBuff +
                (((in_uIndex + in_uCount) > uSize) ? uSize : (in_uIndex + in_uCount));
            const Char* szSrcEnd = this->_cpBuff + uSize;

            while (szSrc <= szSrcEnd) *szDst++ = *szSrc++;

            return *this;
        }

        FixStringBase& FixStringBase::Format(const Char* in_szFormat, ...)
        {
            va_list vlist;
            va_start(vlist, in_szFormat);
            this->FormatV(in_szFormat, vlist);
            va_end(vlist);

            return *this;
        }

        FixStringBase& FixStringBase::FormatV(const Char* in_szFormat, va_list in_vlist)
        {
            if (in_szFormat && this->_uCapacity > 1)
            {
                HE_STR_VSNPRINTF(this->_cpBuff, this->_uCapacity, this->_uCapacity - 1, in_szFormat,
                                 in_vlist);
                this->_cpBuff[this->_uCapacity - 1] = '\0';
            }
            else
            {
                this->Clear();
            }

            return *this;
        }

        Sint32 FixStringBase::Find(const Char* in_szName, Uint32 in_uStart) const
        {
            const Uint32 uLen = this->Length();
            if (uLen <= in_uStart) return -1;

            const Char* szFind = HE_STR_STR(this->_cpBuff + in_uStart, in_szName);
            if (szFind == NULL) return -1;

            return static_cast<Sint32>(szFind - this->_cpBuff);
        }

        const Uint32 FixStringBase::Length() const
        {
#ifdef HE_WIN
            return static_cast<Uint32>(HE_STR_LEN(this->_cpBuff));
#else
            Uint32 uSize = this->Capacity();
            Uint32 uLen  = 0;

            Uint32 uOffset = 0;
            Uint32 i       = 0;
            while (i < uSize)
            {
                Uint8 c = static_cast<Uint8>(this->_cpBuff[i]);
                // 終端があれば終了する
                if (c == 0x00) break;

                // UTF8のコード表を見ると1byteが1文字とか2byteが1文字とかある
                // https://seiai.ed.jp/sys/text/java/utf8table.html
                // 1byteが1文字
                if (c < 0x80) uOffset = 1;
                // 2byteが1文字
                else if (c < 0xE0)
                    uOffset = 2;
                // 3byteが1文字
                else if (c < 0xF0)
                    uOffset = 3;
                // 4byteが1文字
                else if (c < 0xF8)
                    uOffset = 4;
                // 5byteが1文字
                else if (c < 0xFC)
                    uOffset = 5;
                // 6byteが1文字
                else
                    uOffset = 6;

                i += uOffset;
                uLen += 1;
            }

            return uLen;
#endif
        }

        /// <summary>
        /// 文字列をハッシュ化して返す
        /// </summary>
        /// <returns></returns>
        const Uint64 FixStringBase::Hash()
        {
#ifdef HE_WIN
            std::hash<std::wstring> hasher;
#else
            std::hash<std::string> hasher;
#endif
            return hasher(this->Str());
        }

        /// <summary>
        /// UTF8として出力
        /// 文字列をUTF-8として利用したい場合に利用
        /// </summary>
        /// <param name="out"></param>
        /// <param name="in_size"></param>
        void FixStringBase::OutputUTF8(UTF8* out, const Uint32 in_uSize) const
        {
            HE_ASSERT(out);
            HE_ASSERT(in_uSize <= this->Capacity());
            // wchar_t型をutf8のcharに変えて出力
#ifdef HE_WIN
            ::memset(out, 0, in_uSize);
            Sint32 uStrSize = WideCharToMultiByte(CP_UTF8, 0, this->Str(), -1, NULL, 0, NULL, NULL);
            if (uStrSize <= 0) return;

            uStrSize = HE_MIN(static_cast<Uint32>(uStrSize), in_uSize);
            WideCharToMultiByte(CP_UTF8, 0, this->Str(), -1, out, uStrSize, NULL, NULL);
#else
            // TODO: のちに対応
            HE_ASSERT(0 && "Str()のをそのままコピーする");
#endif
        }

        FixStringBase& FixStringBase::_Copy(const Char* in_szName, const Uint32 in_uLen)
        {
            HE_ASSERT(in_szName && "コピーしたい文字列がない");
            HE_ASSERT(0 < this->_uCapacity && "コピー先のバッファサイズがない");
            if (in_szName && 0 < this->_uCapacity)
            {
                HE_STR_ERRNO e = HE_STR_CPY_S(this->_cpBuff, this->_uCapacity, in_szName, in_uLen);
                HE_ASSERT(HE_STR_SUCCESS(e) && "文字列コピーに失敗");
            }
            else
            {
                this->Clear();
            }

            return *this;
        }

        FixStringBase& FixStringBase::_Add(const Char* in_szName)
        {
            if (in_szName && this->_uCapacity > 0)
            {
                Uint32 uLen    = this->Length();
                Sint32 iCatLen = static_cast<Sint32>(this->_uCapacity - uLen - 1);
                HE_ASSERT(0 < iCatLen &&
                          "文字列の長さがバッファサイズを超えて文字列の追加ができない");

                if (iCatLen > 0)
                {
                    HE_STR_ERRNO e = HE_STR_CPY_S(this->_cpBuff + uLen,
                                                  static_cast<Sint32>(this->_uCapacity - uLen),
                                                  in_szName, iCatLen);
                    HE_ASSERT(HE_STR_SUCCESS(e) && "文字列コピーに失敗");

                    this->_cpBuff[this->_uCapacity - 1] = '\0';
                }
            }
            else
            {
                this->Clear();
            }

            return *this;
        }

        FixStringBase& FixStringBase::_Add(Char c)
        {
            Uint32 uLen = this->Length();
            HE_ASSERT(uLen + 1 < this->_uCapacity &&
                      "文字列の長さがバッファサイズを超えて文字の追加ができない");

            if (uLen + 1 < this->_uCapacity)
            {
                this->_cpBuff[uLen]     = c;
                this->_cpBuff[uLen + 1] = '\0';
            }

            return *this;
        }

    }  // namespace Common
}  // namespace Core