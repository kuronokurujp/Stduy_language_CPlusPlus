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

	Code/framework.h
	Code/Resource.h
	Code/targetver.h
	
	Resource/Assets/Locate/JP/Debug.json
	Resource/Assets/Locate/JP/Text.json
	Resource/Assets/Locate/System.toml
	Resource/Assets/UI/Builder/Debug/Launcher.xml
)

