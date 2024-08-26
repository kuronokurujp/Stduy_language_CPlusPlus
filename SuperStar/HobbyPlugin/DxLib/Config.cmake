set(DXLIB_PLUGIN_NAME HoobyEnginePlugin_DXLib)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(DXLIB_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md
    ThirdPartyNotices.txt
    DXLibModule.cpp
    DXLibModule.h

    DxLib/DXLibFile.cpp
    DxLib/DXLibFile.h
    DxLib/DXLibInput.cpp
    DxLib/DXLibInput.h
    DxLib/DXLibTime.cpp
    DxLib/DXLibTime.h
    DxLib/DXLibScreen.h
    DxLib/DXLibScreen.cpp
    DxLib/DXLibSystem.h
    DxLib/DXLibSystem.cpp
)
