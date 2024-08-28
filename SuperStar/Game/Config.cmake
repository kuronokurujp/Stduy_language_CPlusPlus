# プロジェクト名を設定
set(APP_NAME Game)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(TARGET_FILES
	.editorconfig
	README.md

	Code/Main.cpp
	Code/Main.h

	Code/Level/LevelLauncher.cpp
	Code/Level/LevelLauncher.h
	Code/Level/LevelTitle.cpp
	Code/Level/LevelTitle.h
	Code/Level/LevelInGame.cpp
	Code/Level/LevelInGame.h
	Code/Level/LevelInGame/LevelInGame_BG.cpp
	Code/Level/LevelInGame/LevelInGame_BG.h
	
	Code/InGame/Component/InGameSystemComponent.h
	Code/InGame/Component/InGameSystemComponent.cpp
	Code/InGame/Component/InGameShotManagerComponent.cpp
	Code/InGame/Component/InGameShotManagerComponent.h
	Code/InGame/Component/InGameStageManagerComponent.cpp
	Code/InGame/Component/InGameStageManagerComponent.h

	Code/InGame/Actor/Player/InGamePlayerActor.cpp
	Code/InGame/Actor/Player/InGamePlayerActor.h

	Code/InGame/Actor/Enemy/InGameBaseEnemyActor.cpp
	Code/InGame/Actor/Enemy/InGameBaseEnemyActor.h
	Code/InGame/Actor/Enemy/InGameSnakeEnemyActor.cpp
	Code/InGame/Actor/Enemy/InGameSnakeEnemyActor.h
	Code/InGame/Actor/Enemy/InGameBaseEnemyActor.cpp
	Code/InGame/Actor/Enemy/InGameBaseEnemyActor.h
	Code/InGame/Actor/Enemy/InGameSplitEnemyActor.cpp
	Code/InGame/Actor/Enemy/InGameSplitEnemyActor.h
	Code/InGame/Actor/Enemy/InGameZakoEnemyActor.cpp
	Code/InGame/Actor/Enemy/InGameZakoEnemyActor.h

	Code/InGame/Actor/Boss/InGameBoss01Actor.cpp
	Code/InGame/Actor/Boss/InGameBoss01Actor.h
	
	Code/Ingame/InGameActorSystem.h
	Code/Ingame/InGameActorSystem.cpp
	Code/Ingame/InGameActorCommon.h

	Code/framework.h
	Code/Resource.h
	Code/targetver.h
	
	Resource/Assets/Locate/JP/Debug.json
	Resource/Assets/Locate/JP/Text.json
	Resource/Assets/Locate/System.toml
	Resource/Assets/UI/Builder/Debug/Launcher.xml
)

