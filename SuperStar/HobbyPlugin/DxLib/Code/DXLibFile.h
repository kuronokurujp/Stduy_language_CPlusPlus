#pragma once

// DxLibのファイルシステム
#include "Common/CustomMap.h"
#include "File/Path.h"
#include "MiniEngine.h"
#include "Platform/PlatformFile.h"

namespace DXLib
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
        virtual const Bool FileClose(const Core::Common::Handle&) override;

        virtual const Bool FileRead(void* out_pBuff, const Core::Common::Handle& in_rHandle,
                                    const Sint32 in_size) override;
        virtual const Sint32 FileSize(const Core::Common::Handle&) override;

    private:
        /// <summary>
        /// ファイルロードしたハンドルマップ
        /// </summary>
        Core::Common::FixMap<Uint32, Core::Common::Handle, 2048> _mFileHandle;
    };
}  // namespace DXLib
