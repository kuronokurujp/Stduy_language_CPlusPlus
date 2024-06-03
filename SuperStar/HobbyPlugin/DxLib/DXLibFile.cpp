#include "DXLibFile.h"

namespace DxLib {

    TextFileRead::TextFileRead()
    {
    }

    TextFileRead::~TextFileRead()
    {
        this->Close();
    }

    const Bool TextFileRead::Open(Core::File::Path& in_rPath)
    {
        // TODO: ファイルを開く
        if (FileRead_open(in_rPath.Str()) == FALSE)
        {
            return FALSE;
        }

        // TODO: テキストをロード


        this->_bOpen = TRUE;
        return FALSE;
    }

    void TextFileRead::Close()
    {
        this->_bOpen = FALSE;
    }
}
