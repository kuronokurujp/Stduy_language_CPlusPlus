#include "AssetDataBase.h"

#include <fstream>

// 依存モジュール
#include "HobbyEngine/Platform/PlatformFile.h"

namespace AssetManager
{
    void AssetDataBase::_Init(const Char* in_pName, const Core::File::Path& in_rPath)
    {
        E_ASSERT(in_pName && "名前が存在しない");
        E_ASSERT(in_rPath.IsEmpty() == FALSE);

        this->_name = in_pName;
        this->_path = in_rPath;
    }

    const Bool AssetDataToml::_Load(Platform::FileSystemInterface& in_rFileSystem)
    {
        // ファイルロード
        this->_result = toml::parse_file(this->_path.Str());
        if (this->_result.failed())
        {
            Core::Common::FixString256 errorMsg(this->_result.error().description().data());
            E_LOG_LINE(errorMsg.Str());

            return FALSE;
        }

        return TRUE;
    }

    void AssetDataToml::_Unload()
    {
    }

    AssetDataToml::Node AssetDataToml::GetRootNode()
    {
        toml::node& node = this->_result.table();
        return Node(node);
    }

    const Core::Common::FixString512 AssetDataToml::Node::GetString()
    {
        std::optional<std::string_view> s = this->_node.value<std::string_view>();
        return Core::Common::FixString512(s->data());
    }

    AssetDataToml::Node AssetDataToml::Node::_GetNode(const Char* in_pArgs[], const Uint32 in_count)
    {
        toml::node_view<toml::node> node = this->_node;
        for (Uint32 i = 0; i < in_count; ++i)
        {
            node = node.at_path(in_pArgs[i]);
            if (node == FALSE) return Node();
        }

        return Node(node);
    }

    const Bool AssetDataToml::Node::_OutputNodeMap(TOML_NODE_MAP_TYPE* out, const Char* in_pArgs[],
                                                   const Uint32 in_count)
    {
        E_ASSERT(out && "出力するポインターがNULL");
        E_ASSERT(0 < in_count);

        toml::node_view<toml::node> node = this->_node;
        for (Uint32 i = 0; i < in_count; ++i)
        {
            if (E_STR_CMP(in_pArgs[i], E_STR_EMPTY) == 0) break;

            node = node.at_path(in_pArgs[i]);
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

    const Bool AssetDataJson::_Load(Platform::FileSystemInterface& in_rFileSystem)
    {
        Bool bRet = TRUE;

        // ファイルを開く
        this->_fileHandle = in_rFileSystem.FileOpen(this->_path);
        E_ASSERT(this->_fileHandle.Null() == FALSE);
        {
            Byte* pReadTmpBuff = NULL;
            try
            {
                // 開いたファイルのデータサイズを取得して読み込むメモリを確保
                Sint32 size    = in_rFileSystem.FileSize(this->_fileHandle);
                Sint32 memSize = size + 1;
                pReadTmpBuff   = new Byte[memSize];
                ::memset(pReadTmpBuff, '\0', memSize);

                // ファイルの読み込み
                if (in_rFileSystem.FileRead(this->_fileHandle, pReadTmpBuff, size))
                {
                    // 読み込んだメモリをjsonデータとして展開
                    // 展開時にjsonを展開するためのメモリ確保をする
                    pReadTmpBuff[size] = '\n';
                    simdjson::validate_utf8(pReadTmpBuff, memSize);
                    this->_json   = std::make_unique<simdjson::padded_string>(pReadTmpBuff, size);
                    this->_parser = std::make_unique<simdjson::ondemand::parser>(size * 2);
                    {
                        auto resultCode = this->_parser->iterate(*this->_json).get(this->_doc);
                        if (resultCode != simdjson::error_code::SUCCESS)
                        {
                            E_PG_LOG_LINE(E_STR_TEXT("%ls ファイルエラー: %d"), this->_path.Str(),
                                          resultCode);
                            E_LOG_LINE(E_STR_TEXT("エラーのjson内容"));
                            E_LOG_LINE(E_STR_TEXT("%hs"), pReadTmpBuff);

                            bRet = FALSE;
                        }
                    }
                }
                else
                {
                    E_ASSERT(0);
                }
            }
            catch (const simdjson::simdjson_error& e)
            {
                E_PG_LOG_LINE(E_STR_TEXT("{%ls}ファイルの扱いに失敗: %hs"), this->_path.Str(),
                              e.what());
                bRet = FALSE;
            }
            // jsonに展開した時のメモリを利用するので読み込んだメモリを解放
            E_SAFE_DELETE_ARRAY(pReadTmpBuff);
        }
        // ファイルを閉じる
        in_rFileSystem.FileClose(this->_fileHandle);

        if (bRet == FALSE) return FALSE;

        return TRUE;
    }

    void AssetDataJson::_Unload()
    {
        // json展開したメモリを解放
        this->_json.release();
        this->_parser.release();
    }

    const Bool AssetDataJson::_OutputValue(simdjson::fallback::ondemand::value* out,
                                           const Sint32 in_count, const Char* values[])
    {
        E_ASSERT(out);
        E_ASSERT(0 < in_count);

        try
        {
            Core::Common::FixString256 str(values[0]);
            Byte key[256] = {0};
            str.OutputUTF8(key, E_ARRAY_NUM(key));

            auto v = this->_doc.find_field(key);
            E_ASSERT(v.error() == simdjson::error_code::SUCCESS);

            for (Sint32 i = 1; i < in_count; ++i)
            {
                Core::Common::FixString256 str(values[i]);
                str.OutputUTF8(key, E_ARRAY_NUM(key));
                v = v.find_field(key);
                E_ASSERT(v.error() == simdjson::error_code::SUCCESS);
            }
            // 要素を出力
            v.get(*out);

            return TRUE;
        }
        catch (const simdjson::simdjson_error& e)
        {
            E_PG_LOG_LINE(E_STR_TEXT("json要素がない: %hs"), e.what());
        }

        return FALSE;
    }
}  // namespace AssetManager
