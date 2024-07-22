set(ACTOR_PLUGIN_NAME HoobyEnginePlugin_Actor)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(ACTOR_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md

    ActorModule.cpp
    ActorModule.h
    
    Code/Actor.h
    Code/Actor.cpp
    Code/ActorInterface.h
    Code/ActorManager.h
    Code/ActorManager.cpp
    
    Code/Component/Component.h
    Code/Component/Component.cpp
)
