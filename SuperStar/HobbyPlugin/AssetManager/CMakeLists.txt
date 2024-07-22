# プラグインビルドの共通処理.
include(${CMAKE_CURRENT_SOURCE_DIR}/../Plugin.cmake NO_POLICTY_SCOPE)
# Renderプラグインの共通情報.
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICTY_SCOPE)

message("build asset manager plugin lib!")

# 共有ライブラリとして生成するためのオプションの定義.
option(ASSET_MANAGER_PLUGIN_BUILD_SHARED_LIBS "build shared library" OFF)

# プラグイン共通ビルド処理.
common_build_plugin(
    "${ASSET_MANAGER_PLUGIN_NAME}"
    "${ASSET_MANAGER_PLUGIN_TARGET_FILES}"
    "$CACHE{CACHE_ENGINE_INCLUDE_CODE}"
    ${ASSET_MANAGER_PLUGIN_BUILD_SHARED_LIBS}
)