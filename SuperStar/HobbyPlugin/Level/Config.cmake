set(LEVEL_PLUGIN_NAME HoobyEnginePlugin_Level)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(LEVEL_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md
    
    LevelModule.h
    LevelModule.cpp
    
    Code/LevelManager.h
    Code/LevelManager.cpp
    Code/Component/LevelUserInputReceive.h
    Code/Component/LevelUserInputReceive.cpp
)
