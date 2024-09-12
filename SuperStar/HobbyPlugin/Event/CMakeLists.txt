# プラグインビルドの共通処理.
include(${CMAKE_CURRENT_SOURCE_DIR}/../Plugin.cmake NO_POLICTY_SCOPE)
# Renderプラグインの共通情報.
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICTY_SCOPE)
# 共有ライブラリとして生成するためのオプションの定義.
option(EVENT_PLUGIN_BUILD_SHARED_LIBS "build shared library" OFF)
# プラグイン共通ビルド処理.
common_build_plugin("${EVENT_PLUGIN_NAME}" "${EVENT_PLUGIN_TARGET_FILES}" "$CACHE{CACHE_ENGINE_INCLUDE_CODE}" ${EVENT_PLUGIN_BUILD_SHARED_LIBS})
