set(UI_PLUGIN_NAME HoobyEnginePlugin_UI)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(UI_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md

    UIModule.cpp
    UIModule.h
    
    Code/Widget.h
    Code/Widget.cpp
    
    Code/Builder/UILayoutBuilder.h
    Code/Builder/UILayoutBuilder.cpp
    Code/Builder/UILayoutData.h
    Code/Builder/UILayoutData.cpp
    
    Code/Component/Input/UIInputInterface.h
    Code/Component/Input/UIInputRouter.h
    Code/Component/Input/UIInputRouter.cpp
    Code/Component/Input/UIInputTerminalTouch.h
    Code/Component/Input/UIInputTerminalTouch.cpp

    Code/Component/Widget/UIButton.h
    Code/Component/Widget/UIButton.cpp
    Code/Component/Widget/UILayer.h
    Code/Component/Widget/UILayer.cpp
    Code/Component/Widget/UIText.h
    Code/Component/Widget/UIText.cpp
    Code/Component/Widget/UIWidget.h
    Code/Component/Widget/UIWidget.cpp
)
