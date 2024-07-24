# プラグインビルドの共通処理.
include(${CMAKE_CURRENT_SOURCE_DIR}/../Plugin.cmake NO_POLICTY_SCOPE)
# DXLibプラグインの共通情報.
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICTY_SCOPE)

message("build dxlib plugin lib!")

# 共有ライブラリとして生成するためのオプションの定義.
option(DXLIB_PLUGIN_BUILD_SHARED_LIBS "build shared library" OFF)

# プラグイン共通ビルド処理.
common_build_plugin(${DXLIB_PLUGIN_NAME} "${DXLIB_PLUGIN_TARGET_FILES}" "$CACHE{CACHE_ENGINE_INCLUDE_CODE}" ${DXLIB_PLUGIN_BUILD_SHARED_LIBS})

# DXLibのライブラリディレクトリを設定
target_link_directories(${DXLIB_PLUGIN_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/DxLib/ThirdParty/DxLib_VC/lib)

target_include_directories(${DXLIB_PLUGIN_NAME} 
    PRIVATE
        # DXLibのインクルードディレクトリ.
        ${CMAKE_CURRENT_SOURCE_DIR}/DxLib/ThirdParty/DxLib_VC/lib
        # プラグインに依存しているプラグインコード
        # ライブラリのリンクはcmakeで指定したCMakeLists.txtでやっている
        ${CMAKE_CURRENT_SOURCE_DIR}/../Render/
)
