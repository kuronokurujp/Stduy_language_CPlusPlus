#pragma once

// TODO: DxLibのファイルシステム
#include "MiniEngine.h"
#include "Core/File/Path.h"

#include "Platform/PlatformFile.h"

#include "DxLib.h"

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
    };
}
