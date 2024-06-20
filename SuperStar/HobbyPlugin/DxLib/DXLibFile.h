﻿#pragma once

// TODO: DxLibのファイルシステム
#include "Core/Common/FixMap.h"
#include "Core/File/Path.h"
#include "DxLib.h"
#include "MiniEngine.h"
#include "Platform/PlatformFile.h"

namespace DxLib
{
    /// <summary>
    /// DxLibのファイルシステムを使ったテキストファイル読み込み
    /// </summary>
    class TextFileRead : public Platform::TextFileReadInterface
    {
    public:
        TextFileRead();
        virtual ~TextFileRead();

        virtual const Bool Open(Core::File::Path&) override;
        virtual void Close() override;
    };

    /// <summary>
    ///  DxLibのファイルシステム
    /// </summary>
    class FileSystem : public Platform::FileSystemInterface
    {
    public:
        virtual const Core::Common::Handle FileOpen(const Core::File::Path& in_rPath) override;
        virtual const Bool FileClose(const Core::Common::Handle& in_rHandle) override;

        virtual const Bool FileRead(const Core::Common::Handle& in_rHandle, void* out_pBuff,
                                    const Sint32 in_size) override;
        virtual const Sint32 FileSize(const Core::Common::Handle& in_rHandle) override;

    private:
        /// <summary>
        /// ファイルロードしたハンドルマップ
        /// </summary>
        Core::Common::FixMap<Uint32, Core::Common::Handle, 2048> _fileHandleMap;
    };
}  // namespace DxLib
