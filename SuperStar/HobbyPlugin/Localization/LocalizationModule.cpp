#include "LocalizationModule.h"

#include "HobbyEngine/ThirdParty/tomlplusplus/toml.hpp"
#include "HobbyEngine/ThirdParty/nlohmann/json.hpp"

namespace Localization
{
    const Bool LocalizationModule::Init()
    {
        return TRUE;
    }

    const Bool LocalizationModule::End()
    {
        return TRUE;
    }

    const Bool LocalizationModule::Update(const Float32 in_deltaTime)
    {
        return TRUE;
    }

    const Bool LocalizationModule::LoadSystemFile(const Core::Common::FixString128& in_rFilePath)
    {
        // TODO: tomlファイルかチェック
        // TEST
        auto config = toml::parse_file(in_rFilePath.Str());
        std::string_view s = config["config"]["current_dir"].value_or("");

        // TODO: tomlファイルをロード
        return TRUE;
    }

    const Bool LocalizationModule::UnloadFile()
    {
        return TRUE;
    }
 
    const Bool LocalizationModule::LoadTextGroup(const Core::Common::FixString128& in_rGroupName)
    {
        // TODO: グループ情報からロードするjsonファイルパスを生成
        // TODO: jsonファイルかチェック
        // TODO: jsonファイルをロード
        return TRUE;
    }

    const Bool LocalizationModule::UnloadTextGroup(const Core::Common::FixString128& in_rGroupName)
    {
        return TRUE;
    }

    Core::Common::FixString1024 LocalizationModule::Text(
        const Core::Common::FixString128& in_rGropuName,
        const Core::Common::FixString128 in_key)
    {
        return Core::Common::FixString1024();
    }
}

