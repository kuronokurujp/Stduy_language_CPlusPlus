#include "AssetDataBase.h"

// 依存モジュール
#include "HobbyEngine/Platform/PlatformModule.h"

// tomlライブラリ内の例外処理をOFF
// こちらでエラー処理をする
#define TOML_EXCEPTIONS 0
#include "ThirdParty/tomlplusplus/toml.hpp"
#include "ThirdParty/nlohmann/json.hpp"

using json = nlohmann::json;

namespace AssetManager
{
    void AssetDataBase::_Init(const Char* in_pName, Core::File::Path& in_rPath)
    {
        this->_name = in_pName;
        this->_path = in_rPath;
    }

    void AssetDataToml::_Init(const Char* in_pName, Core::File::Path& in_rPath)
    {
        this->_name = in_pName;
        this->_path = in_rPath;
    }

    const Bool AssetDataToml::_Load(Platform::FileSystemInterface& in_rFileSystem)
    {
        // TODO: ファイルロード
        toml::parse_result parseResult = toml::parse_file(this->_path.Str());
        if (parseResult.failed())
        {
            Core::Common::FixString256 errorMsg(parseResult.error().description().data());
            E_LOG_LINE(errorMsg.Str());

            return FALSE;
        }

        return TRUE;
    }

    void AssetDataToml::_Unload()
    {
        // TODO: ファイルをアンロード
    }

    void AssetDataJson::_Init(const Char* in_pName, Core::File::Path& in_rPath)
    {
        this->_name = in_pName;
        this->_path = in_rPath;
    }

    const Bool AssetDataJson::_Load(Platform::FileSystemInterface& in_rFileSystem)
    {
        // TODO: ファイルロード
        return FALSE;
    }

    void AssetDataJson::_Unload()
    {
        // TODO: ファイルをアンロード
    }

}
