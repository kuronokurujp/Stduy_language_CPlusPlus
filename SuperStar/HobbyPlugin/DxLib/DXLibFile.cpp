#include "DXLibFile.h"

#include "DxLib.h"

namespace DxLib
{

    TextFileRead::TextFileRead()
    {
    }

    TextFileRead::~TextFileRead()
    {
        this->Close();
    }

    const Bool TextFileRead::Open(Core::File::Path& in_rPath)
    {
        // ファイルを開く
        if (FileRead_open(in_rPath.Str()) == FALSE)
        {
            return FALSE;
        }

        this->_bOpen = TRUE;
        return FALSE;
    }

    void TextFileRead::Close()
    {
        this->_bOpen = FALSE;
    }

    const Core::Common::Handle FileSystem::FileOpen(const Core::File::Path& in_rPath)
    {
        auto handle = FileRead_open(in_rPath.Str(), FALSE);
        if (handle == 0) return Core::Common::Handle();

        Core::Common::Handle fileHandle;
        fileHandle.Init(handle);
        this->_fileHandleMap.Add(fileHandle.Magic(), fileHandle);

        return fileHandle;
    }

    const Bool FileSystem::FileRead(const Core::Common::Handle& in_rHandle, void* out_pBuff,
                                    const Sint32 in_size)
    {
        E_ASSERT(in_rHandle.Null() == FALSE && "ファイルのハンドルがない");
        E_ASSERT(out_pBuff != NULL && "ファイルの読み込みのバッファの先頭アドレスがない");
        E_ASSERT(0 < in_size && "ファイルの読み込みサイズが0以下");

        const Sint32 resultCode = FileRead_read(out_pBuff, in_size, in_rHandle.Index());
        if (resultCode == -1)
        {
            E_ASSERT(0 && "ファイルの読み込み失敗");
            return FALSE;
        }

        return TRUE;
    }

    const Sint32 FileSystem::FileSize(const Core::Common::Handle& in_rHandle)
    {
        E_ASSERT(in_rHandle.Null() == FALSE);
        // ファイル読み込みのサイズがSint32なので合わせる
        return static_cast<Sint32>(FileRead_size_handle(in_rHandle.Index()));
    }

    const Bool FileSystem::FileClose(const Core::Common::Handle& in_rHandle)
    {
        if (in_rHandle.Null()) return FALSE;

        this->_fileHandleMap.Erase(in_rHandle.Magic());
        if (FileRead_close(in_rHandle.Index()) != 0) return FALSE;

        return TRUE;
    }
}  // namespace DxLib
