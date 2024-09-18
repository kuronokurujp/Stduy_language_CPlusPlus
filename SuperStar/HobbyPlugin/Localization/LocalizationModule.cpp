#include "LocalizationModule.h"

namespace Localization
{
    LocalizationModule::LocalizationModule() : ModuleBase(ModuleName())
    {
        this->_AppendDependenceModule<AssetManager::AssetManagerModule>();
    }

    Bool LocalizationModule::_VStart()
    {
        return TRUE;
    }

    Bool LocalizationModule::_VRelease()
    {
        // ロードしたアセットを全て破棄
        for (auto it = this->_locateDataMap.Begin(); it != this->_locateDataMap.End(); ++it)
        {
            this->UnloadTextAll(it->key);
        }

        auto pLocalModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pLocalModule);

        pLocalModule->Unload(this->_sysAssetDataHandle);

        return TRUE;
    }

    Bool LocalizationModule::_VUpdate(const Float32 in_fDeltaTime)
    {
        return TRUE;
    }

    Bool LocalizationModule::LoadSystemFile(const Core::Common::StringBase& in_szrFilePath)
    {
        auto pLocalModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pLocalModule);

        // tomlファイルをロード
        auto szAssetName = HE_STR_TEXT("LSystem");

        this->_sysAssetDataHandle =
            pLocalModule->Load<SystemAssetData>(szAssetName, in_szrFilePath.Str());
        HE_ASSERT(this->_sysAssetDataHandle.Null() == FALSE);
        if (this->_sysAssetDataHandle.Null()) return FALSE;

        return TRUE;
    }

    Bool LocalizationModule::UnloadSystemFile()
    {
        auto pLocalModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pLocalModule);

        pLocalModule->Unload(this->_sysAssetDataHandle);
        return TRUE;
    }

    Bool LocalizationModule::LoadTextAll(const Core::Common::StringBase& in_szLocateName)
    {
        auto pLocalModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pLocalModule);

        SystemAssetData& a = pLocalModule->GetAsset<SystemAssetData>(this->_sysAssetDataHandle);
        const SystemAssetData::LocateMap& map = a.FindLocate(in_szLocateName.Str());
        HE_ASSERT(map.Empty() == FALSE);

        LocateTextMap textMap;
        Core::Common::FixString128 szTmp;
        for (auto it = map.Begin(); it != map.End(); ++it)
        {
            szTmp = in_szLocateName.Str();
            szTmp.Concatenate(HE_STR_TEXT("/"), it->key.Str());

            auto h = pLocalModule->Load<LocateAssetData>(szTmp.Str(), it->data._textFilePath.Str());
            textMap.Add(it->key, h);
        }
        HE_ASSERT(0 < textMap.Size());

        this->_locateDataMap.Add(in_szLocateName.Str(), textMap);

        return TRUE;
    }

    Bool LocalizationModule::UnloadTextAll(const Core::Common::StringBase& in_szLocateName)
    {
        auto pLocalModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pLocalModule);

        auto pMap = &this->_locateDataMap[in_szLocateName.Str()];
        for (auto it = pMap->Begin(); it != pMap->End(); ++it)
        {
            pLocalModule->Unload(it->data);
        }
        pMap->Clear();

        return TRUE;
    }

    const Char* LocalizationModule::Text(const Core::Common::StringBase& in_szLocateName,
                                         const Core::Common::StringBase& in_szGroupName,
                                         const Core::Common::StringBase& in_szKey)
    {
        auto pLocalModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pLocalModule);

        auto locateIter = this->_locateDataMap.FindKey(in_szLocateName.Str());
        HE_ASSERT(locateIter.IsValid());

        auto groupIter = locateIter->data.FindKey(in_szGroupName.Str());
        HE_ASSERT(groupIter.IsValid());

        LocateAssetData& rData = pLocalModule->GetAsset<LocateAssetData>(groupIter->data);
        // テキストを取得
        return rData.GetText(in_szKey.Str()).Str();
    }

    Bool SystemAssetData::_VLoad(Platform::FileInterface& in_rFileSystem)
    {
        const Bool bRet = AssetManager::AssetDataToml::_VLoad(in_rFileSystem);
        HE_ASSERT(bRet);

        auto rootNode = this->GetRootNode();

        // コンフィグデータからデータパスのディレクトリ名を取得
        Core::Common::FixString512 pathName =
            rootNode.GetNode(HE_STR_TEXT("config"), HE_STR_TEXT("current_dir")).GetString();
        Core::File::Path dataRootPath(pathName.Str());

        // Locateデータ構築
        AssetManager::AssetDataToml::Node locateNode = rootNode.GetNode(HE_STR_TEXT("locate"));

        AssetManager::AssetDataToml::ToolNodeMapType mLocateNode;
        AssetManager::AssetDataToml::ToolNodeMapType mGroupNode;

        locateNode.OutputNodeMap(&mLocateNode, HE_STR_EMPTY);

        Core::Common::FixString128 locateStr;
        for (auto it = mLocateNode.Begin(); it != mLocateNode.End(); ++it)
        {
            locateStr = it->key.Str();
            locateStr.ToUpper();

            Core::Common::CustomFixMap<Core::Common::FixString128, LocateData, 32> mData;

            mGroupNode.Clear();
            it->data.OutputNodeMap(&mGroupNode, HE_STR_EMPTY);

            for (auto groupIt = mGroupNode.Begin(); groupIt != mGroupNode.End(); ++groupIt)
            {
                // ファイルパス連結する
                Core::File::Path path(dataRootPath);
                path += Core::File::Path(locateStr.Str());
                path +=
                    Core::File::Path(groupIt->data.GetNode(HE_STR_TEXT("json")).GetString().Str());

                LocateData data(path);
                HE_LOG_LINE(HE_STR_TEXT("%s"), data._textFilePath.Str());

                const Core::Common::FixString128 szGroupName(groupIt->key.Str());
                mData.Add(szGroupName, data);
            }

            this->_locateDataMap.Add(locateStr, mData);
        }

        return TRUE;
    }

    const SystemAssetData::LocateMap& SystemAssetData::FindLocate(const Char* in_szLocateName)
    {
        HE_ASSERT(in_szLocateName);

        Core::Common::FixString128 key(in_szLocateName);
        auto it = this->_locateDataMap.FindKey(key);
        return it->data;
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
