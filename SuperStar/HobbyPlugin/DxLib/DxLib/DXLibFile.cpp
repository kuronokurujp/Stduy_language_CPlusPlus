#include "DXLibFile.h"

#include "DxLib.h"

namespace DXLib
{
    const Core::Common::Handle File::FileOpen(const Core::File::Path& in_rPath)
    {
        auto handle = FileRead_open(in_rPath.Str(), FALSE);
        if (handle == 0) return Core::Common::Handle();

        Core::Common::Handle fileHandle;
        fileHandle.Init(handle);
        this->_mFileHandle.Add(fileHandle.Magic(), fileHandle);

        return fileHandle;
    }

    const Bool File::FileRead(void* out_pBuff, const Core::Common::Handle& in_rHandle,
                              const Sint32 in_size)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE && "ファイルのハンドルがない");
        HE_ASSERT(out_pBuff != NULL && "ファイルの読み込みのバッファの先頭アドレスがない");
        HE_ASSERT(0 < in_size && "ファイルの読み込みサイズが0以下");

        const Sint32 resultCode = FileRead_read(out_pBuff, in_size, in_rHandle.Index());
        if (resultCode == -1)
        {
            HE_ASSERT(0 && "ファイルの読み込み失敗");
            return FALSE;
        }

        return TRUE;
    }

    const Sint32 File::FileSize(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);
        // ファイル読み込みのサイズがSint32なので合わせる
        return static_cast<Sint32>(FileRead_size_handle(in_rHandle.Index()));
    }

    const Bool File::FileClose(const Core::Common::Handle& in_rHandle)
    {
        if (in_rHandle.Null()) return FALSE;

        this->_mFileHandle.Erase(in_rHandle.Magic());
        if (FileRead_close(in_rHandle.Index()) != 0) return FALSE;

        return TRUE;
    }
}  // namespace DXLib
