#pragma once

// プラットフォームのファイルシステムインターフェイス

namespace Platform
{
    class TextFileReadInterface
    {
    public:
        virtual const Bool Open(Core::File::Path&) = 0;
        virtual void Close() = 0;

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
    };
}
