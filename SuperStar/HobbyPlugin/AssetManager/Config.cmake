set(ASSET_MANAGER_PLUGIN_NAME HoobyEnginePlugin_AssetManager)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(ASSET_MANAGER_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md

    AssetManagerModule.cpp
    AssetManagerModule.h
    
    Code/AssetDataBase.h
    Code/AssetDataBase.cpp
    
    ThirdParty/pugixml/pugiconfig.hpp
    ThirdParty/pugixml/pugixml.cpp
    ThirdParty/pugixml/pugixml.hpp
    ThirdParty/simidjson/simdjson.h
    ThirdParty/simidjson/simdjson.cpp
    ThirdParty/tomlplusplus/toml.hpp
)
