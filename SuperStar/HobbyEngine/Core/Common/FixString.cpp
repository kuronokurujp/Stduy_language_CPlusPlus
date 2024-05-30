#include "FixString.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef _WIN

#include <windows.h>

#endif

namespace Core
{
    namespace Common
    {
        FixStringBase::FixStringBase()
        {
            static Char	s_InitialBuff[] = E_STR_TEXT("test");
            this->_Init(s_InitialBuff, 0);
        }

        FixStringBase::FixStringBase(Char* in_pBuff, Uint32 in_size)
        {
            this->_Init(in_pBuff, in_size);
        }

        void FixStringBase::_Init(Char* in_pBuff, Uint32 in_size)
        {
            E_ASSERT(in_pBuff && in_size);

            this->_pBuff = in_pBuff;
            this->_size = in_size;
            this->_pBuff[0] = '\0';
#ifdef _WIN
        
#endif
        }

        FixStringBase& FixStringBase::Replace(const Char* in_pOld, const Char* in_pNew)
        {
            if (in_pOld == NULL)
                return *this;

            if (in_pNew == NULL)
                return *this;

            Uint32 oldLength = static_cast<Uint32>(E_STR_LEN(in_pOld));
            Ptr newLength = static_cast<Ptr>(E_STR_LEN(in_pNew));
            const Char* pSrcStr = this->_pBuff;

            // 対象を探す
            do
            {
                const Char* pFind = E_STR_STR(pSrcStr, in_pOld);
                if (!pFind)	break;

                // 見つかった場所を抜いて新しい文字列に差し替え
                Ptr index = pFind - this->_pBuff;
                Remove(static_cast<Uint32>(index), oldLength);
                Insert(static_cast<Uint32>(index), in_pNew);
                pSrcStr = this->_pBuff + index + newLength;

                // もうこれ以上置き換えられない
                if (static_cast<Uint32>(pSrcStr - this->_pBuff) >= this->_size)
                    break;

            } while (*pSrcStr != '\0');

            return *this;
        }

        FixStringBase& FixStringBase::Insert(Uint32 in_index, const Char* in_pInsertStr)
        {
            Char* pBuffEnd = this->_pBuff + this->_size - 1;
            Uint32 originLen = this->Size();
            Uint32 insertLen = ((!in_pInsertStr) ? 1 : static_cast<Uint32>(E_STR_LEN(in_pInsertStr)));

            // 後ろに追加して終わり
            if (in_index >= originLen)
                return (*this) += in_pInsertStr;

            // 挿入サイズをチェックする
            if (in_index + insertLen >= this->_size - 1)
                insertLen = this->_size - in_index - 1;

            // 挿入箇所以降の文字を後ろへスライド
            {
                const Char *pSrcTop = this->_pBuff + in_index;
                Char *pSrcTail = this->_pBuff + originLen;
                Char *pDst = this->_pBuff + originLen + insertLen;
                const Char *pSrc;

                // 溢れチェック
                if (pDst > pBuffEnd)
                {
                    Ptr cutLen = pDst - pBuffEnd;

                    pDst -= cutLen;
                    pSrcTail -= cutLen;
                    *pSrcTail = '\0';
                }
                E_ASSERT(*pSrcTail == '\0');
                
                // コピーする
                for (pSrc = pSrcTail; pSrc >= pSrcTop; --pSrc, --pDst)
                    *pDst = *pSrc;
            }
            
            // 挿入処理
            {
                Char* pDst = this->_pBuff + in_index;
                const Char* pSrc = in_pInsertStr;

                if (in_pInsertStr)
                {
                    for (Uint32 i=0; i < insertLen; ++i)
                        *pDst++ = *pSrc++;
                }
                else
                {
                    *pDst++ = E_STR_TEXT('\0');
                }
            }

            return *this;
        }

        FixStringBase& FixStringBase::Remove(Uint32 in_index, Uint32 in_count)
        {
            Uint32 fullLen = this->Size();
            Char *pDst = this->_pBuff + ((in_index > fullLen) ? fullLen : in_index);
            const Char *pSrc = this->_pBuff + (((in_index + in_count) > fullLen) ? fullLen : (in_index + in_count));
            const Char *pSrcEnd = this->_pBuff + fullLen;

            while (pSrc <= pSrcEnd)
                *pDst++ = *pSrc++;

            return *this;
        }

        FixStringBase& FixStringBase::Format(const Char *in_pFormat, ...)
        {
            va_list vlist;
            va_start(vlist, in_pFormat);
            this->FormatV(in_pFormat, vlist);
            va_end(vlist);

            return *this;
        }

        FixStringBase& FixStringBase::FormatV(const Char* in_pFormat, va_list in_vList)
        {
            if (in_pFormat && this->_size > 1)
            {
                E_STR_VSNPRINTF(this->_pBuff, this->_size, this->_size - 1, in_pFormat, in_vList);
                this->_pBuff[this->_size - 1] = '\0';
            }
            else
            {
                this->Empty();
            }
            
            return *this;
        }

        Sint32 FixStringBase::Find(const Char* in_pStr, Uint32 in_start) const
        {
            const Uint32 len = this->Size();
            if (len <= in_start)
                return -1;

            const Char *pFind = E_STR_STR(this->_pBuff + in_start, in_pStr);
            if (pFind == NULL)
                return -1;

            return static_cast<Sint32>(pFind - this->_pBuff);
        }

        Uint32 FixStringBase::Length()
        {
#ifdef _WIN
            return this->Size();
#else
            Uint32 size = this->Size();
            Uint32 len = 0;

            Uint32 offset = 0;
            Uint32 i = 0;
            while (i < size)
            {
                Uint8 c = static_cast<Uint8>(this->_pBuff[i]);
                // UTF8のコード表を見ると1byteが1文字とか2byteが1文字とかある
                // https://seiai.ed.jp/sys/text/java/utf8table.html
                // 1byteが1文字
                if (c < 0x80)
                    offset = 1;
                // 2byteが1文字
                else if (c < 0xE0)
                    offset = 2;
                // 3byteが1文字
                else if (c < 0xF0)
                    offset = 3;
                // 4byteが1文字
                else if (c < 0xF8)
                    offset = 4;
                // 5byteが1文字
                else if (c < 0xFC)
                    offset = 5;
                // 6byteが1文字
                else
                    offset = 6;

                i += offset;
                len += 1;
            }

            return len;
#endif
        }

        Bool FixStringBase::operator==(const Char* in_pStr) const
        {
            return (in_pStr && (E_STR_CMP(this->_pBuff, in_pStr) == 0));
        }

        FixStringBase& FixStringBase::Copy(const Char* in_pStr)
        {
            if (in_pStr && this->_size > 0)
            {
                E_STR_CPY_S(this->_pBuff, this->_size, in_pStr, this->_size - 1);
                this->_pBuff[this->_size - 1] = '\0';
            }
            else
            {
                this->Empty();
            }
            
            return *this;
        }

        FixStringBase& FixStringBase::Add(const Char* in_pStr)
        {
            if (in_pStr && this->_size > 0)
            {
                Uint32 len = this->Size();
                Uint32 catLen = this->_size - len - 1;

                if (catLen > 0)
                {
                    E_STR_CPY_S(this->_pBuff + len, this->_size - len, in_pStr, catLen);
                }
                this->_pBuff[_size - 1] = '\0';
            }
            else
            {
                Empty();
            }

            return *this;
        }

        FixStringBase& FixStringBase::Add(Char c)
        {
            Uint32 len = this->Size();

            if (len + 1 < this->_size)
            {
                this->_pBuff[len] = c;
                this->_pBuff[len + 1] = '\0';
            }

            return *this;
        }

#ifdef _WIN
        void ByteToWideChar(Char* out_wides, const Byte in_bytes[], const Uint32 in_wideSize)
        {
            MultiByteToWideChar(CP_ACP, 0, in_bytes, -1, out_wides, in_wideSize);
        }

        Char* ByteToWideCharByTemp(const Byte in_bytes[])
        {
            static Char c[256] = {};
            ByteToWideChar(c, in_bytes, 256);
            return c;
        }
#endif
    }
}

