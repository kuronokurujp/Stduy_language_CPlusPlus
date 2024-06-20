#pragma once

#include "Core/Common/Handle.h"
#include "Core/File/Path.h"
#include "MiniEngine.h"

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
        const Core::Common::FixString256& ErrorMsg() { return this->_err; }

    protected:
        Core::Common::FixString256 _err;
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
        virtual const Bool FileRead(const Core::Common::Handle& in_rHandle, void* out_pBuff,
                                    const Sint32 in_size)                             = 0;

        virtual const Sint32 FileSize(const Core::Common::Handle& in_rHandle) = 0;
    };
}  // namespace Platform
