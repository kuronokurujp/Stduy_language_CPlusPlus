set(ACTOR_PLUGIN_NAME HoobyEnginePlugin_Actor)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(ACTOR_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md

    ActorModule.cpp
    ActorModule.h
    
    Actor/Actor.h
    Actor/Actor.cpp
    Actor/ActorInterface.h
    Actor/ActorManager.h
    Actor/ActorManager.cpp
    
    Actor/Component/Component.h
    Actor/Component/Component.cpp
)
