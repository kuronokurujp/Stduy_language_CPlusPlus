#include "LocalizationModule.h"

MODULE_GENRATE_DEFINITION(Localization::LocalizationModule, Localization);

namespace Localization
{
    const Bool LocalizationModule::Start()
    {
        return TRUE;
    }

    const Bool LocalizationModule::Release()
    {
        // ロードしたアセットを全て破棄
        for (auto it = this->_locateDataMap.Begin(); it != this->_locateDataMap.End(); ++it)
        {
            this->UnloadTextAll(it->_tKey);
        }

        ModuleAssetManager()->Unload(this->_sysAssetDataHandle);

        return TRUE;
    }

    const Bool LocalizationModule::Update(const Float32 in_fDeltaTime)
    {
        return TRUE;
    }

    const Bool LocalizationModule::LoadSystemFile(const Core::Common::FixStringBase& in_szrFilePath)
    {
        // tomlファイルをロード
        auto szAssetName = HE_STR_TEXT("LSystem");
        this->_sysAssetDataHandle =
            ModuleAssetManager()->Load<SystemAssetData>(szAssetName, in_szrFilePath.Str());
        HE_ASSERT(this->_sysAssetDataHandle.Null() == FALSE);
        if (this->_sysAssetDataHandle.Null()) return FALSE;

        return TRUE;
    }

    const Bool LocalizationModule::UnloadSystemFile()
    {
        ModuleAssetManager()->Unload(this->_sysAssetDataHandle);
        return TRUE;
    }

    const Bool LocalizationModule::LoadTextAll(const Core::Common::FixStringBase& in_szLocateName)
    {
        SystemAssetData& a =
            ModuleAssetManager()->GetAsset<SystemAssetData>(this->_sysAssetDataHandle);
        const SYSTEM_ASSET_LOCATE_MAP& map = a.FindLocate(in_szLocateName.Str());
        HE_ASSERT(map.Empty() == FALSE);

        LOCATE_TEXT_MAP textMap;
        Core::Common::FixString128 szTmp;
        for (auto it = map.Begin(); it != map.End(); ++it)
        {
            szTmp = in_szLocateName.Str();
            szTmp.Concatenate(HE_STR_TEXT("/"), it->_tKey.Str());

            auto h = ModuleAssetManager()->Load<LocateAssetData>(szTmp.Str(),
                                                                 it->_tData._textFilePath.Str());
            textMap.Add(it->_tKey, h);
        }
        HE_ASSERT(0 < textMap.Size());

        this->_locateDataMap.Add(in_szLocateName.Str(), textMap);

        return TRUE;
    }

    const Bool LocalizationModule::UnloadTextAll(const Core::Common::FixStringBase& in_szLocateName)
    {
        auto pMap = &this->_locateDataMap[in_szLocateName.Str()];
        for (auto it = pMap->Begin(); it != pMap->End(); ++it)
        {
            ModuleAssetManager()->Unload(it->_tData);
        }
        pMap->Clear();

        return TRUE;
    }

    const Char* LocalizationModule::Text(const Core::Common::FixStringBase& in_szLocateName,
                                         const Core::Common::FixStringBase& in_szGroupName,
                                         const Core::Common::FixStringBase& in_szKey)
    {
        auto locateIter = this->_locateDataMap.Find(in_szLocateName.Str());
        HE_ASSERT(locateIter.IsValid());

        auto groupIter = locateIter->_tData.Find(in_szGroupName.Str());
        HE_ASSERT(groupIter.IsValid());

        LocateAssetData& rData = ModuleAssetManager()->GetAsset<LocateAssetData>(groupIter->_tData);
        // テキストを取得
        return rData.GetText(in_szKey.Str()).Str();
    }

    const Bool SystemAssetData::_Load(Platform::FileSystemInterface& in_rFileSystem)
    {
        const Bool bRet = AssetManager::AssetDataToml::_Load(in_rFileSystem);
        HE_ASSERT(bRet);

        auto rootNode = this->GetRootNode();

        // コンフィグデータからデータパスのディレクトリ名を取得
        Core::Common::FixString512 pathName =
            rootNode.GetNode(HE_STR_TEXT("config"), HE_STR_TEXT("current_dir")).GetString();
        Core::File::Path dataRootPath(pathName.Str());

        // Locateデータ構築
        AssetManager::AssetDataToml::Node locateNode = rootNode.GetNode(HE_STR_TEXT("locate"));

        TOML_NODE_MAP_TYPE mLocateNode;
        TOML_NODE_MAP_TYPE mGroupNode;
        locateNode.OutputNodeMap(&mLocateNode, HE_STR_EMPTY);
        for (auto it = mLocateNode.Begin(); it != mLocateNode.End(); ++it)
        {
            Core::Common::FixString128 locateStr(it->_tKey.Str());
            locateStr.ToUpper();

            Core::Common::FixMap<Core::Common::FixString128, LocateData, 32> mData;

            mGroupNode.Clear();
            it->_tData.OutputNodeMap(&mGroupNode, HE_STR_EMPTY);

            for (auto groupIt = mGroupNode.Begin(); groupIt != mGroupNode.End(); ++groupIt)
            {
                // ファイルパス連結する
                Core::File::Path path(dataRootPath);
                path += Core::File::Path(locateStr.Str());
                path += Core::File::Path(
                    groupIt->_tData.GetNode(HE_STR_TEXT("json")).GetString().Str());

                LocateData data(path);
                HE_LOG_LINE(HE_STR_FORMAT_TEXT, data._textFilePath.Str());

                const Core::Common::FixString128 szGroupName(groupIt->_tKey.Str());
                mData.Add(szGroupName, data);
            }

            this->_locateDataMap.Add(locateStr, mData);
        }

        return TRUE;
    }

    const SYSTEM_ASSET_LOCATE_MAP& SystemAssetData::FindLocate(const Char* in_szLocateName)
    {
        HE_ASSERT(in_szLocateName);

        Core::Common::FixString128 key(in_szLocateName);
        auto it = this->_locateDataMap.Find(key);
        return it->_tData;
    }

    const Core::Common::FixString1024& LocateAssetData::GetText(const Char* in_szKey)
    {
        // キャッシュしているテキストがなければデータからテキストを取る
        if (this->_textBuffMap.Contains(in_szKey) == FALSE)
        {
            simdjson::fallback::ondemand::value v;
            if (this->_OutputValue(&v, in_szKey))
            {
                auto value = v["items"]["0"]["text"];
                HE_ASSERT(value.error() == simdjson::error_code::SUCCESS);
                HE_ASSERT(value.is_string());

                auto str   = value.get_string();
                auto str_v = str.value();
                this->_textBuffMap.Add(in_szKey, value.get_string().value());
            }
        }

        return this->_textBuffMap[in_szKey];
    }
}  // namespace Localization
