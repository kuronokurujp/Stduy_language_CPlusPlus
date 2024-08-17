set(UI_PLUGIN_NAME HoobyEnginePlugin_UI)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(UI_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md

    UIModule.cpp
    UIModule.h
    
    UI/Widget.h
    UI/Widget.cpp
    
    UI/Builder/UILayoutBuilder.h
    UI/Builder/UILayoutBuilder.cpp
    UI/Builder/UILayoutData.h
    UI/Builder/UILayoutData.cpp
    
    UI/Component/Input/UIInputRouter.h
    UI/Component/Input/UIInputRouter.cpp

    UI/Component/Widget/UIButton.h
    UI/Component/Widget/UIButton.cpp
    UI/Component/Widget/UILayer.h
    UI/Component/Widget/UILayer.cpp
    UI/Component/Widget/UIText.h
    UI/Component/Widget/UIText.cpp
    UI/Component/Widget/UIWidget.h
    UI/Component/Widget/UIWidget.cpp
)
