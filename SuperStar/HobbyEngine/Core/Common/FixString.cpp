#include "FixString.h"

#include <stdarg.h>
#include <stdio.h>

#ifdef _WIN

#include <windows.h>

#endif

namespace Core
{
    namespace Common
    {
        FixStringBase::FixStringBase(Char* in_pBuff, Uint32 in_size)
        {
            this->_Init(in_pBuff, in_size);
        }

        void FixStringBase::_Init(Char* in_pBuff, Uint32 in_size)
        {
            E_ASSERT(in_pBuff && in_size);

            this->_pBuff    = in_pBuff;
            this->_capacity = in_size;
            this->_pBuff[0] = E_STR_TEXT('\0');
        }

        FixStringBase& FixStringBase::Replace(const Char* in_pOld, const Char* in_pNew)
        {
            if (in_pOld == NULL) return *this;

            if (in_pNew == NULL) return *this;

            Uint32 oldLength    = static_cast<Uint32>(E_STR_LEN(in_pOld));
            Ptr newLength       = static_cast<Ptr>(E_STR_LEN(in_pNew));
            const Char* pSrcStr = this->_pBuff;

            // 対象を探す
            do
            {
                const Char* pFind = E_STR_STR(pSrcStr, in_pOld);
                if (!pFind) break;

                // 見つかった場所を抜いて新しい文字列に差し替え
                Ptr index = pFind - this->_pBuff;
                Remove(static_cast<Uint32>(index), oldLength);
                Insert(static_cast<Uint32>(index), in_pNew);
                pSrcStr = this->_pBuff + index + newLength;

                // もうこれ以上置き換えられない
                if (static_cast<Uint32>(pSrcStr - this->_pBuff) >= this->_capacity) break;

            } while (*pSrcStr != E_STR_TEXT('\0'));

            return *this;
        }

        FixStringBase& FixStringBase::Insert(Uint32 in_index, const Char* in_pInsertStr)
        {
            Char* pBuffEnd   = this->_pBuff + this->_capacity - 1;
            Uint32 originLen = this->Length();
            Uint32 insertLen =
                ((!in_pInsertStr) ? 1 : static_cast<Uint32>(E_STR_LEN(in_pInsertStr)));

            // 後ろに追加して終わり
            if (in_index >= originLen) return (*this) += in_pInsertStr;

            // 挿入サイズをチェックする
            if (in_index + insertLen >= this->_capacity - 1)
                insertLen = this->_capacity - in_index - 1;

            // 挿入箇所以降の文字を後ろへスライド
            {
                const Char* pSrcTop = this->_pBuff + in_index;
                Char* pSrcTail      = this->_pBuff + originLen;
                Char* pDst          = this->_pBuff + originLen + insertLen;
                const Char* pSrc;

                // 溢れチェック
                if (pDst > pBuffEnd)
                {
                    Ptr cutLen = pDst - pBuffEnd;

                    pDst -= cutLen;
                    pSrcTail -= cutLen;
                    *pSrcTail = E_STR_TEXT('\0');
                }
                E_ASSERT(*pSrcTail == E_STR_TEXT('\0'));

                // コピーする
                for (pSrc = pSrcTail; pSrc >= pSrcTop; --pSrc, --pDst) *pDst = *pSrc;
            }

            // 挿入処理
            {
                Char* pDst       = this->_pBuff + in_index;
                const Char* pSrc = in_pInsertStr;

                if (in_pInsertStr)
                {
                    for (Uint32 i = 0; i < insertLen; ++i) *pDst++ = *pSrc++;
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
            Uint32 size = this->Capacity();
            Char* pDst  = this->_pBuff + ((in_index > size) ? size : in_index);
            const Char* pSrc =
                this->_pBuff + (((in_index + in_count) > size) ? size : (in_index + in_count));
            const Char* pSrcEnd = this->_pBuff + size;

            while (pSrc <= pSrcEnd) *pDst++ = *pSrc++;

            return *this;
        }

        FixStringBase& FixStringBase::Format(const Char* in_pFormat, ...)
        {
            va_list vlist;
            va_start(vlist, in_pFormat);
            this->FormatV(in_pFormat, vlist);
            va_end(vlist);

            return *this;
        }

        FixStringBase& FixStringBase::FormatV(const Char* in_pFormat, va_list in_vList)
        {
            if (in_pFormat && this->_capacity > 1)
            {
                E_STR_VSNPRINTF(this->_pBuff, this->_capacity, this->_capacity - 1, in_pFormat,
                                in_vList);
                this->_pBuff[this->_capacity - 1] = '\0';
            }
            else
            {
                this->Clear();
            }

            return *this;
        }

        Sint32 FixStringBase::Find(const Char* in_pStr, Uint32 in_start) const
        {
            const Uint32 len = this->Length();
            if (len <= in_start) return -1;

            const Char* pFind = E_STR_STR(this->_pBuff + in_start, in_pStr);
            if (pFind == NULL) return -1;

            return static_cast<Sint32>(pFind - this->_pBuff);
        }

        const Uint32 FixStringBase::Length() const
        {
#ifdef _WIN
            return static_cast<Uint32>(E_STR_LEN(this->_pBuff));
#else
            Uint32 size = this->Capacity();
            Uint32 len  = 0;

            Uint32 offset = 0;
            Uint32 i      = 0;
            while (i < size)
            {
                Uint8 c = static_cast<Uint8>(this->_pBuff[i]);
                // 終端があれば終了する
                if (c == 0x00) break;

                // UTF8のコード表を見ると1byteが1文字とか2byteが1文字とかある
                // https://seiai.ed.jp/sys/text/java/utf8table.html
                // 1byteが1文字
                if (c < 0x80) offset = 1;
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

        /// <summary>
        /// 文字列をハッシュ化して返す
        /// </summary>
        /// <returns></returns>
        const Uint64 FixStringBase::Hash()
        {
#ifdef _WIN
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
        void FixStringBase::OutputUTF8(Byte* out, Uint32 in_size) const
        {
            E_ASSERT(out);
            E_ASSERT(in_size <= this->Capacity());
            // wchar_t型をutf8のcharに変えて出力
#ifdef _WIN
            ::memset(out, 0, in_size);
            Sint32 StrSize = WideCharToMultiByte(CP_UTF8, 0, this->Str(), -1, NULL, 0, NULL, NULL);
            if (StrSize <= 0) return;

            StrSize = E_MIN(static_cast<Uint32>(StrSize), in_size);
            WideCharToMultiByte(CP_UTF8, 0, this->Str(), -1, out, StrSize, NULL, NULL);
#else
            // TODO: のちに対応
            E_ASSERT(0 && "Str()のをそのままコピーする");
#endif
        }

        Bool FixStringBase::operator==(const Char* in_pStr) const
        {
            return (in_pStr && (E_STR_CMP(this->_pBuff, in_pStr) == 0));
        }

        FixStringBase& FixStringBase::_Copy(const Char* in_pStr, const Uint32 in_strLen)
        {
            E_ASSERT(in_pStr && "コピーしたい文字列がない");
            E_ASSERT(0 < this->_capacity && "コピー先のバッファサイズがない");
            if (in_pStr && 0 < this->_capacity)
            {
                E_STR_ERRNO e = E_STR_CPY_S(this->_pBuff, this->_capacity, in_pStr, in_strLen);
                E_ASSERT(E_STR_SUCCESS(e) && "文字列コピーに失敗");
            }
            else
            {
                this->Clear();
            }

            return *this;
        }

        FixStringBase& FixStringBase::_Add(const Char* in_pStr)
        {
            if (in_pStr && this->_capacity > 0)
            {
                Uint32 len    = this->Length();
                Sint32 catLen = static_cast<Sint32>(this->_capacity - len - 1);
                E_ASSERT(0 < catLen &&
                         "文字列の長さがバッファサイズを超えて文字列の追加ができない");

                if (catLen > 0)
                {
                    E_STR_ERRNO e =
                        E_STR_CPY_S(this->_pBuff + len, static_cast<Sint32>(this->_capacity - len),
                                    in_pStr, catLen);
                    E_ASSERT(E_STR_SUCCESS(e) && "文字列コピーに失敗");

                    this->_pBuff[this->_capacity - 1] = '\0';
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
            Uint32 len = this->Length();
            E_ASSERT(len + 1 < this->_capacity &&
                     "文字列の長さがバッファサイズを超えて文字の追加ができない");

            if (len + 1 < this->_capacity)
            {
                this->_pBuff[len]     = c;
                this->_pBuff[len + 1] = '\0';
            }

            return *this;
        }

    }  // namespace Common
}  // namespace Core
