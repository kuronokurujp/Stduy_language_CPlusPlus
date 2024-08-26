﻿#pragma once

// DxLibのファイルシステム
#include "Engine/Common/CustomMap.h"
#include "Engine/File/Path.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformFile.h"

namespace DXLib
{
    /// <summary>
    ///  DxLibのファイルシステム
    /// </summary>
    class File : public Platform::FileInterface
    {
    public:
        virtual const Core::Common::Handle FileOpen(const Core::File::Path& in_rPath) override;
        virtual const Bool FileClose(const Core::Common::Handle&) override;

        virtual const Bool FileRead(void* out_pBuff, const Core::Common::Handle& in_rHandle,
                                    const Sint32 in_size) override;
        virtual const Sint32 FileSize(const Core::Common::Handle&) override;

    private:
        /// <summary>
        /// ファイルロードしたハンドルマップ
        /// </summary>
        Core::Common::CustomFixMap<Uint32, Core::Common::Handle, 2048> _mFileHandle;
    };
}  // namespace DXLib
