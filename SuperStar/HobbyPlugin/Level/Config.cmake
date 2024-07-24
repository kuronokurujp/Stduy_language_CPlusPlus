set(LEVEL_PLUGIN_NAME HoobyEnginePlugin_Level)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(LEVEL_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md
    
    LevelModule.h
    LevelModule.cpp
    
    Level/LevelManager.h
    Level/LevelManager.cpp
    Level/Component/LevelUserInputReceive.h
    Level/Component/LevelUserInputReceive.cpp
)
