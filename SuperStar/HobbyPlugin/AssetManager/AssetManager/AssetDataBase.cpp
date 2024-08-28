#include "AssetDataBase.h"

#include <fstream>

// 依存モジュール
#include "Engine/Platform/PlatformFile.h"

namespace AssetManager
{
    void AssetDataBase::_VInit(const Char* in_szName, const Core::File::Path& in_rPath)
    {
        HE_ASSERT(in_szName && "名前が存在しない");
        HE_ASSERT(in_rPath.Empty() == FALSE);

        this->_szName = in_szName;
        this->_path   = in_rPath;
    }

    const Bool AssetDataToml::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        // ファイルロード
        this->_result = toml::parse_file(this->_path.Str());
        if (this->_result.failed())
        {
            // エラーログを出してアサートで止める
            Core::Common::FixString256 errorMsg(this->_result.error().description().data());
            HE_LOG_LINE(HE_STR_FORMAT_TEXT, errorMsg.Str());
            HE_ASSERT(FALSE);

            return FALSE;
        }

        return TRUE;
    }

    void AssetDataToml::_VUnload()
    {
    }

    AssetDataToml::Node AssetDataToml::GetRootNode()
    {
        toml::node& rNode = this->_result.table();
        return Node(rNode);
    }

    const Core::Common::FixString512 AssetDataToml::Node::GetString() const
    {
        std::optional<std::string_view> s = this->_node.value<std::string_view>();
        return Core::Common::FixString512(s->data());
    }

    AssetDataToml::Node AssetDataToml::Node::_GetNode(const Char* in_szaName[],
                                                      const Uint32 in_uCount)
    {
        toml::node_view<toml::node> node = this->_node;
        for (Uint32 i = 0; i < in_uCount; ++i)
        {
            node = node.at_path(in_szaName[i]);
            if (node == FALSE) return Node();
        }

        return Node(node);
    }

    const Bool AssetDataToml::Node::_OutputNodeMap(ToolNodeMapType* out, const Char* in_szaName[],
                                                   const Uint32 in_uCount)
    {
        HE_ASSERT(out && "出力するポインターがNULL");
        HE_ASSERT(0 < in_uCount);

        toml::node_view<toml::node> node = this->_node;
        for (Uint32 i = 0; i < in_uCount; ++i)
        {
            if (HE_STR_CMP(in_szaName[i], HE_STR_EMPTY) == 0) break;

            node = node.at_path(in_szaName[i]);
            if (node == FALSE) return FALSE;
        }

        // 指定したノードの中に複数のノードがあれば設定する
        if (node.is_table() == FALSE) return FALSE;

        auto table = node.as_table();
        for (auto it = table->begin(); it != table->end(); ++it)
        {
            out->Add(it->first.data(), Node(it->second));
        }

        return TRUE;
    }

    const Bool AssetDataJson::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        Bool bRet = TRUE;

        // ファイルを開く
        this->_fileHandle = in_rFileSystem.VFileOpen(this->_path);
        HE_ASSERT(this->_fileHandle.Null() == FALSE);
        {
            UTF8* pReadTmpBuff = NULL;
            try
            {
                // 開いたファイルのデータサイズを取得して読み込むメモリを確保
                Sint32 iSize    = in_rFileSystem.VFileSize(this->_fileHandle);
                Sint32 iMemSize = iSize + 1;
                pReadTmpBuff    = HE_NEW_ARRAY(UTF8, iMemSize, 0);  // new UTF8[iMemSize];
                ::memset(pReadTmpBuff, '\0', iMemSize);

                // ファイルの読み込み
                if (in_rFileSystem.VFileRead(pReadTmpBuff, this->_fileHandle, iSize))
                {
                    // 読み込んだメモリをjsonデータとして展開
                    // 展開時にjsonを展開するためのメモリ確保をする
                    pReadTmpBuff[iSize] = '\n';
                    simdjson::validate_utf8(pReadTmpBuff, iMemSize);
                    this->_json   = std::make_unique<simdjson::padded_string>(pReadTmpBuff, iSize);
                    this->_parser = std::make_unique<simdjson::ondemand::parser>(iSize * 2);
                    {
                        auto resultCode = this->_parser->iterate(*this->_json).get(this->_doc);
                        if (resultCode != simdjson::error_code::SUCCESS)
                        {
                            HE_PG_LOG_LINE(HE_STR_FORMAT_TEXT HE_STR_TEXT(" ファイルエラー: %d"),
                                           this->_path.Str(), resultCode);
                            HE_LOG_LINE(HE_STR_TEXT("エラーのjson内容"));
                            HE_LOG_LINE(HE_STR_FORMAT_PURE_TEXT, pReadTmpBuff);

                            bRet = FALSE;
                        }
                    }
                }
                else
                {
                    HE_ASSERT(0);
                }
            }
            catch (const simdjson::simdjson_error& e)
            {
                HE_PG_LOG_LINE(HE_STR_FORMAT_TEXT HE_STR_TEXT("ファイルの扱いに失敗: ")
                                   HE_STR_FORMAT_PURE_TEXT,
                               this->_path.Str(), e.what());
                bRet = FALSE;
            }
            // jsonに展開した時のメモリを利用するので読み込んだメモリを解放
            HE_SAFE_DELETE_ARRAY(pReadTmpBuff);
        }
        // ファイルを閉じる
        in_rFileSystem.VFileClose(this->_fileHandle);

        if (bRet == FALSE) return FALSE;

        return TRUE;
    }

    void AssetDataJson::_VUnload()
    {
        // json展開したメモリを解放
        this->_json.release();
        this->_parser.release();
    }

    const Bool AssetDataJson::_OutputValue(simdjson::fallback::ondemand::value* out,
                                           const Char* in_szaName[], const Uint32 in_uCount)
    {
        HE_ASSERT(out);
        HE_ASSERT(0 < in_uCount);

        try
        {
            Core::Common::FixString256 str(in_szaName[0]);
            UTF8 szKey[256] = {0};
            str.OutputUTF8(szKey, HE_ARRAY_SIZE(szKey));

            auto v = this->_doc[szKey];
            HE_ASSERT(v.error() == simdjson::error_code::SUCCESS);

            // TODO: 未対応
            // 複数の取得は未対応
            for (Uint32 i = 1; i < in_uCount; ++i)
            {
                HE_ASSERT(FALSE);
                /*
                Core::Common::FixString256 str(in_szaName[i]);
                str.OutputUTF8(szKey, HE_ARRAY_SIZE(szKey));
                v = v[szKey];
                HE_ASSERT(v.error() == simdjson::error_code::SUCCESS);
                */
            }
            // 要素を出力
            v.get(*out);

            return TRUE;
        }
        catch (const simdjson::simdjson_error& e)
        {
            HE_PG_LOG_LINE(HE_STR_TEXT("json要素がない: ") HE_STR_FORMAT_PURE_TEXT, e.what());
        }

        return FALSE;
    }

    const Bool AssetDataXml::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        Bool bRet = TRUE;

        // ファイルを開く
        this->_fileHandle = in_rFileSystem.VFileOpen(this->_path);
        HE_ASSERT(this->_fileHandle.Null() == FALSE);
        {
            UTF8* pReadTmpBuff = NULL;
            {
                // 開いたファイルのデータサイズを取得して読み込むメモリを確保
                Sint32 iSize    = in_rFileSystem.VFileSize(this->_fileHandle);
                Sint32 iMemSize = iSize + 1;
                pReadTmpBuff    = HE_NEW_ARRAY(UTF8, iMemSize, 0);  // new UTF8[iMemSize];
                ::memset(pReadTmpBuff, '\0', iMemSize);

                // ファイルの読み込み
                if (in_rFileSystem.VFileRead(pReadTmpBuff, this->_fileHandle, iSize))
                {
                    pugi::xml_parse_result result = this->_doc.load_string(pReadTmpBuff);
                    if (result)
                    {
                        HE_LOG_LINE(HE_STR_TEXT("XML Load Success: ") HE_STR_FORMAT_TEXT,
                                    this->_path.Str());
                    }
                    else
                    {
                        HE_LOG_LINE(HE_STR_TEXT("Error description: ") HE_STR_FORMAT_PURE_TEXT,
                                    result.description());
                        HE_LOG_LINE(HE_STR_TEXT("Error offset: %td (error at [... ")
                                        HE_STR_FORMAT_PURE_TEXT HE_STR_TEXT("] "),
                                    result.offset, (pReadTmpBuff + result.offset));
                    }
                }
                else
                {
                    HE_ASSERT(0);
                }
            }
            // 展開した時のメモリを利用するので読み込んだメモリを解放
            HE_SAFE_DELETE_ARRAY(pReadTmpBuff);
        }
        // ファイルを閉じる
        in_rFileSystem.VFileClose(this->_fileHandle);

        if (bRet == FALSE) return FALSE;

        return TRUE;
    }

    void AssetDataXml::_VUnload()
    {
        this->_doc.reset();
    }

}  // namespace AssetManager
