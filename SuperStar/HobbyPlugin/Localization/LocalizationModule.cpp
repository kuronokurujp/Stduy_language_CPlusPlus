#include "LocalizationModule.h"

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
        if (in_rFilePath.IsEmpty())
            return FALSE;

        // tomlファイルをロード
        const Core::Common::Handle& handle = AssetManager::AssetManagerModule::I().Load<SystemAssetData>(E_STR_TEXT("LSystem"), Core::File::Path(in_rFilePath.Str()));
        if (handle.Null())
        {
            return FALSE;
        }

        /*

        toml::parse_result parseResult = toml::parse_file(in_rFilePath.Str());
        if (parseResult.failed())
        {
            Core::Common::FixString256 errorMsg(parseResult.error().description().data());
            E_LOG_LINE(errorMsg.Str());

            return FALSE;
        }


        auto configData = parseResult["config"];
        std::optional<std::string_view> s = configData["current_dir"].value<std::string_view>();
        Core::File::Path dataRootPath(s->data());

        for (auto&& [k, v] : parseResult)
        {
            // [a.b.c]だとaがキー名として出力された
            // 入れ子になっているのかな？
            Core::Common::FixString256 s(k.data());
            if (s == E_STR_TEXT("locate"))
            {
                if (v.is_table())
                {
                    auto locateTable = v.as_table();
                    for (auto locateIt = locateTable->begin(); locateIt != locateTable->end(); ++locateIt)
                    {
                        Core::Common::FixString128 locateStr(locateIt->first.data());
                        locateStr.ToUpper();

                        Core::Common::FixMap<Core::Common::FixString128, LocateData, 32> dataMap;

                        auto groupTable = locateIt->second.as_table();
                        for (auto groupIt = groupTable->begin(); groupIt != groupTable->end(); ++groupIt)
                        {
                            // ファイルパス連結する
                            Core::File::Path path(dataRootPath);
                            path += Core::File::Path(locateStr.Str());
                            path += Core::File::Path(groupIt->second.at_path(E_STR_TEXT("json")).as_string()->get().c_str());

                            LocateData data(path);
                            E_LOG_LINE(data._textFilePath.Str());

                            const Core::Common::FixString128 groupStr(groupIt->first.data());
                            dataMap.Add(groupStr, data);
                        }

                        this->_locateDataMap.Add(locateStr, dataMap);
                    }
                }
            }
        }
        */

        return TRUE;
    }

    const Bool LocalizationModule::UnloadSystemFile()
    {
        return TRUE;
    }
 
    const Bool LocalizationModule::LoadTextAll(const Core::Common::FixString128& in_rLocateName)
    {
        // TODO: グループ情報からロードするjsonファイルパスを生成
        auto iter = this->_locateDataMap.Find(in_rLocateName);
        E_ASSERT(iter.IsValid() && "ロケートのデータがあるか");

        // TODO: 全グループのテキストをロードする
        auto end = iter->_data.End();
        for (auto groupIter = iter->_data.Begin(); groupIter != end; ++groupIter)
        {
            // TODO: jsonファイルかチェック
            // LocateText a = AssetManager.Load<LocateText>("jp","test");

            //std::ifstream f( "example.json");
            //json data = json::parse(f);
            // TODO: jsonファイルをロード
            // groupIter->_data._textFilePath
        }

        return TRUE;
    }

    const Bool LocalizationModule::UnloadTextAll(const Core::Common::FixString128& in_rLocateName)
    {
        return TRUE;
    }

    Core::Common::FixString1024 LocalizationModule::Text(
        const Core::Common::FixString128& in_rLocateName,
        const Core::Common::FixString128& in_rGropuName,
        const Core::Common::FixString128& in_rKey)
    {
        return Core::Common::FixString1024();
    }
}

