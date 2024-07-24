#pragma once

#include "Engine/Common/Handle.h"
#include "Engine/Core.h"
#include "Engine/File/Path.h"

// プラットフォームのファイルシステムインターフェイス

namespace Platform
{
    class TextFileReadInterface
    {
    public:
        virtual const Bool Open(Core::File::Path&) = 0;
        virtual void Close()                       = 0;

        /// <summary>
        /// 失敗した時のエラーメッセージ
        /// </summary>
        /// <returns></returns>
        const Core::Common::FixString256& ErrorMsg() { return this->_szErr; }

    protected:
        Core::Common::FixString256 _szErr;
        Bool _bOpen = FALSE;
    };

    /// <summary>
    /// ファイルシステム管理のインターフェイス
    /// </summary>
    class FileSystemInterface
    {
    public:
        virtual const Core::Common::Handle FileOpen(const Core::File::Path& in_rPath) = 0;
        virtual const Bool FileClose(const Core::Common::Handle& in_rHandle)          = 0;
        virtual const Bool FileRead(void* out_pBuff, const Core::Common::Handle& in_rHandle,
                                    const Sint32 in_uSize)                            = 0;

        virtual const Sint32 FileSize(const Core::Common::Handle& in_rHandle) = 0;
    };
}  // namespace Platform
