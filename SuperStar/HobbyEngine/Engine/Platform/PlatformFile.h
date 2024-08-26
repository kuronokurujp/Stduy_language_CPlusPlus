#pragma once

#include "Engine/Common/Handle.h"
#include "Engine/Core.h"
#include "Engine/File/Path.h"

// プラットフォームのファイルシステムインターフェイス

namespace Platform
{
    /// <summary>
    /// ファイルシステム管理のインターフェイス
    /// </summary>
    class FileInterface
    {
    public:
        virtual const Core::Common::Handle FileOpen(const Core::File::Path& in_rPath) = 0;
        virtual const Bool FileClose(const Core::Common::Handle& in_rHandle)          = 0;
        virtual const Bool FileRead(void* out_pBuff, const Core::Common::Handle& in_rHandle,
                                    const Sint32 in_uSize)                            = 0;

        virtual const Sint32 FileSize(const Core::Common::Handle& in_rHandle) = 0;
    };
}  // namespace Platform
