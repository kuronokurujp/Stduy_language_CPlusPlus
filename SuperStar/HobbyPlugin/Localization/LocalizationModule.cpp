#include "LocalizationModule.h"

MODULE_GENRATE_DEFINITION(Localization::LocalizationModule, Localization);

namespace Localization
{
    const Bool LocalizationModule::Init()
    {
        return TRUE;
    }

    const Bool LocalizationModule::End()
    {
        // ロードしたアセットを全て破棄
        for (auto it = this->_locateDataMap.Begin(); it != this->_locateDataMap.End(); ++it)
        {
            this->UnloadTextAll(it->_key);
        }

        ModuleAssetManager().Unload(this->_sysAssetDataHandle);

        return TRUE;
    }

    const Bool LocalizationModule::Update(const Float32 in_deltaTime)
    {
        return TRUE;
    }

    const Bool LocalizationModule::LoadSystemFile(const Core::Common::FixString128& in_rFilePath)
    {
        // tomlファイルをロード
        auto assetName = E_STR_TEXT("LSystem");
        this->_sysAssetDataHandle =
            ModuleAssetManager().Load<SystemAssetData>(assetName,
                                                       Core::File::Path(in_rFilePath.Str()));
        E_ASSERT(this->_sysAssetDataHandle.Null() == FALSE);
        if (this->_sysAssetDataHandle.Null()) return FALSE;

        return TRUE;
    }

    const Bool LocalizationModule::UnloadSystemFile()
    {
        ModuleAssetManager().Unload(this->_sysAssetDataHandle);
        return TRUE;
    }

    const Bool LocalizationModule::LoadTextAll(const Core::Common::FixString128& in_rLocateName)
    {
        SystemAssetData& a =
            ModuleAssetManager().GetAsset<SystemAssetData>(this->_sysAssetDataHandle);
        const SYSTEM_ASSET_LOCATE_MAP& map = a.FindLocate(in_rLocateName.Str());
        E_ASSERT(map.Empty() == FALSE);

        LOCATE_TEXT_MAP textMap;
        Core::Common::FixString128 tmpStr;
        for (auto it = map.Begin(); it != map.End(); ++it)
        {
            tmpStr = in_rLocateName;
            tmpStr.Concatenate(E_STR_TEXT("/"), it->_key.Str());

            auto h =
                ModuleAssetManager().Load<LocateAssetData>(tmpStr.Str(), it->_data._textFilePath);
            textMap.Add(it->_key, h);
        }
        E_ASSERT(0 < textMap.Size());

        this->_locateDataMap.Add(in_rLocateName, textMap);

        return TRUE;
    }

    const Bool LocalizationModule::UnloadTextAll(const Core::Common::FixString128& in_rLocateName)
    {
        auto map = this->_locateDataMap[in_rLocateName];
        for (auto it = map.Begin(); it != map.End(); ++it)
        {
            ModuleAssetManager().Unload(it->_data);
        }
        map.Clear();

        return TRUE;
    }

    const Char* LocalizationModule::Text(const Core::Common::FixString128& in_rLocateName,
                                         const Core::Common::FixString128& in_rGroupName,
                                         const Core::Common::FixString128& in_rKey)
    {
        auto it = this->_locateDataMap.Find(in_rLocateName);
        E_ASSERT(it.IsValid());

        auto itGroup = it->_data.Find(in_rGroupName);
        E_ASSERT(itGroup.IsValid());

        LocateAssetData& data = ModuleAssetManager().GetAsset<LocateAssetData>(itGroup->_data);
        // テキストを取得
        return data.GetText(in_rKey.Str()).Str();
    }

    const Bool SystemAssetData::_Load(Platform::FileSystemInterface& in_rFileSystem)
    {
        const Bool bRet = AssetManager::AssetDataToml::_Load(in_rFileSystem);
        E_ASSERT(bRet);

        auto rootNode = this->GetRootNode();

        // コンフィグデータからデータパスのディレクトリ名を取得
        Core::Common::FixString512 path =
            rootNode.GetNode(E_STR_TEXT("config"), E_STR_TEXT("current_dir")).GetString();
        Core::File::Path dataRootPath(path.Str());

        // Locateデータ構築
        AssetManager::AssetDataToml::Node locateNode = rootNode.GetNode(E_STR_TEXT("locate"));

        TOML_NODE_MAP_TYPE locateMap;
        TOML_NODE_MAP_TYPE groupMap;
        locateNode.OutputNodeMap(&locateMap, E_STR_EMPTY);
        for (auto it = locateMap.Begin(); it != locateMap.End(); ++it)
        {
            Core::Common::FixString128 locateStr(it->_key.Str());
            locateStr.ToUpper();

            Core::Common::FixMap<Core::Common::FixString128, LocateData, 32> dataMap;

            groupMap.Clear();
            it->_data.OutputNodeMap(&groupMap, E_STR_EMPTY);

            for (auto groupIt = groupMap.Begin(); groupIt != groupMap.End(); ++groupIt)
            {
                // ファイルパス連結する
                Core::File::Path path(dataRootPath);
                path += Core::File::Path(locateStr.Str());
                path +=
                    Core::File::Path(groupIt->_data.GetNode(E_STR_TEXT("json")).GetString().Str());

                LocateData data(path);
                E_LOG_LINE(data._textFilePath.Str());

                const Core::Common::FixString128 groupStr(groupIt->_key.Str());
                dataMap.Add(groupStr, data);
            }

            this->_locateDataMap.Add(locateStr, dataMap);
        }

        return TRUE;
    }

    const SYSTEM_ASSET_LOCATE_MAP& SystemAssetData::FindLocate(const Char* in_pLocateName)
    {
        E_ASSERT(in_pLocateName);

        Core::Common::FixString128 key(in_pLocateName);
        auto it = this->_locateDataMap.Find(key);
        return it->_data;
    }

    const Core::Common::FixString1024& LocateAssetData::GetText(const Char* in_pKey)
    {
        // キャッシュしているテキストがなければデータからテキストを取る
        if (this->_textBuffMap.Contains(in_pKey) == FALSE)
        {
            simdjson::fallback::ondemand::value v;
            if (this->_OutputValue(&v, in_pKey))
            {
                auto value = v["items"]["0"]["text"];
                E_ASSERT(value.error() == simdjson::error_code::SUCCESS);
                E_ASSERT(value.is_string());

                auto str   = value.get_string();
                auto str_v = str.value();
                this->_textBuffMap.Add(in_pKey, value.get_string().value());
            }
        }

        return this->_textBuffMap[in_pKey];
    }
}  // namespace Localization
