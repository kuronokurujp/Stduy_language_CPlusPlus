# プラグインビルドの共通処理.
include(${CMAKE_CURRENT_SOURCE_DIR}/../Plugin.cmake NO_POLICTY_SCOPE)
# Renderプラグインの共通情報.
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICTY_SCOPE)

message("build ui plugin lib!")

# 共有ライブラリとして生成するためのオプションの定義.
option(UI_PLUGIN_BUILD_SHARED_LIBS "build shared library" OFF)

# プラグイン共通ビルド処理.
common_build_plugin("${UI_PLUGIN_NAME}" "${UI_PLUGIN_TARGET_FILES}" "$CACHE{CACHE_ENGINE_INCLUDE_CODE}" ${UI_PLUGIN_BUILD_SHARED_LIBS})

target_include_directories(${UI_PLUGIN_NAME} 
    PRIVATE
        # プラグインに依存しているプラグインコード
        # ライブラリのリンクはcmakeで指定したCMakeLists.txtでやっている
        ${CMAKE_CURRENT_SOURCE_DIR}/../Actor/
        ${CMAKE_CURRENT_SOURCE_DIR}/../AssetManager/
        ${CMAKE_CURRENT_SOURCE_DIR}/../Render/
        ${CMAKE_CURRENT_SOURCE_DIR}/../Localization/
        ${CMAKE_CURRENT_SOURCE_DIR}/../Level/
)