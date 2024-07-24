set(RENDER_PLUGIN_NAME HoobyEnginePlugin_Render)

# プロジェクトに登録するファイル群
# ここに全て記載する
set(RENDER_PLUGIN_TARGET_FILES
    .editorconfig
    LICENSE.txt
    README.md

    RenderModule.cpp
    RenderModule.h
    
    Render/Color.h
    Render/CommandBuffer.h

    Render/Command/Command.h
    Render/Command/Command.cpp
)
