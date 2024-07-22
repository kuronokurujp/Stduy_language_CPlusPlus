# プラグインビルドの共通処理.
include(${CMAKE_CURRENT_SOURCE_DIR}/../Plugin.cmake NO_POLICTY_SCOPE)
# Renderプラグインの共通情報.
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICTY_SCOPE)

message("build level plugin lib!")

# 共有ライブラリとして生成するためのオプションの定義.
option(LEVEL_PLUGIN_BUILD_SHARED_LIBS "build shared library" OFF)

# プラグイン共通ビルド処理.
common_build_plugin("${LEVEL_PLUGIN_NAME}" "${LEVEL_PLUGIN_TARGET_FILES}" "$CACHE{CACHE_ENGINE_INCLUDE_CODE}" ${LEVEL_PLUGIN_BUILD_SHARED_LIBS})

target_include_directories(${LEVEL_PLUGIN_NAME} 
    PRIVATE
        # プラグインに依存しているプラグインコード
        # ライブラリのリンクはcmakeで指定したCMakeLists.txtでやっている
        ${CMAKE_CURRENT_SOURCE_DIR}/../Actor/
)