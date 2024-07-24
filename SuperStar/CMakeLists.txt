cmake_minimum_required(VERSION 3.26)

# 共通情報を取得
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICTY_SCOPE)
# 各プロジェクトの共通情報を取得
# ゲームアプリ
include(${CMAKE_CURRENT_SOURCE_DIR}/Game/Config.cmake NO_POLICTY_SCOPE)
# ゲームエンジン.
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyEngine/Config.cmake NO_POLICTY_SCOPE)
# DXLibプラグイン.
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/DxLib/Config.cmake NO_POLICTY_SCOPE)
# 描画プラグイン.
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/Render/Config.cmake NO_POLICTY_SCOPE)
# アクタープラグイン.
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/Actor/Config.cmake NO_POLICTY_SCOPE)
# アセット管理プラグイン
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/AssetManager/Config.cmake NO_POLICTY_SCOPE)
# レベルプラグイン
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/Level/Config.cmake NO_POLICTY_SCOPE)
# ローカライズプラグイン
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/Localization/Config.cmake NO_POLICTY_SCOPE)
# UIプラグイン
include(${CMAKE_CURRENT_SOURCE_DIR}/HobbyPlugin/UI/Config.cmake NO_POLICTY_SCOPE)

option(
  CORE_CRL_MD
  "Use Cubism Core that is multithread-specific and DLL-specific version"
  OFF
)

# プロジェクトで参照するファイル群
# プロジェクトにグループ分けするためにキャッシュ変数にしてグローバル参照できるようにしている
set(PROJECT_CACHE_SRC_FILES CACHE INTERNAL description)
set(PROJECT_CACHE_INC_FILES CACHE INTERNAL description)

# ワークスペース名と使用言語C++をプロジェクトに設定.
project(${WORKSPACE_NAME} VERSION 1.0 LANGUAGES CXX)

# ビルド成果物を出力するディレクトリ指定.
# 実行ファイルの出力先
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/${WORKSPACE_NAME})
# 静的ライブラリファイルの出力先
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/${WORKSPACE_NAME})
# 共有ライブラリの出力先
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin/${WORKSPACE_NAME})

# Set configuration (Release and Debug only).
set(CMAKE_CONFIGURATION_TYPES Debug Release
  CACHE STRING "Configurations" FORCE
)
# Suppress generation of ZERO_CHECK project.
set(CMAKE_SUPPRESS_REGENERATION ON)

# makeコマンド実行時のビルドステップの詳細表示.
set(CMAKE_VERBOSE_MAKEFILE ON)

# 警告とすべてエラーとして扱う.
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# C++コンパイラの設定.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Cコンパイラの設定.
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# アーキテクチャ指定.
if(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64")
  set(ARCH x86_64)
  set(DX_ARCH x64)
elseif(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:X86")
  set(ARCH x86)
  set(DX_ARCH Win32)
else()
  message(FATAL_ERROR "[${WORKSPACE_NAME}] Invalid linker flag ${CMAKE_EXE_LINKER_FLAGS}.")
endif()

# VSコンパイラを指定.
if(MSVC_VERSION GREATER_EQUAL 1930 AND MSVC_VERSION LESS 1950)
  # Visual Studio 2022
  set(COMPILER 143)
  set(MSVC_YEAR 2022)
elseif(MSVC)
  message(FATAL_ERROR "[${WORKSPACE_NAME}] Unsupported Visual C++ compiler used.")
else()
  message(FATAL_ERROR "[${WORKSPACE_NAME}] Unsupported compiler used.")
endif()

# 各プロジェクトを設定
add_subdirectory(HobbyEngine EXCLUDE_FROM_ALL)
add_subdirectory(HobbyPlugin/DxLib EXCLUDE_FROM_ALL)
add_subdirectory(HobbyPlugin/Render EXCLUDE_FROM_ALL)
add_subdirectory(HobbyPlugin/Actor EXCLUDE_FROM_ALL)
add_subdirectory(HobbyPlugin/AssetManager EXCLUDE_FROM_ALL)
add_subdirectory(HobbyPlugin/Level EXCLUDE_FROM_ALL)
add_subdirectory(HobbyPlugin/Localization EXCLUDE_FROM_ALL)
add_subdirectory(HobbyPlugin/UI EXCLUDE_FROM_ALL)

# テストプロジェクトを作る
include(${CMAKE_CURRENT_SOURCE_DIR}/Test/Config.cmake NO_POLICTY_SCOPE)

add_subdirectory(Game)

# スタートプロジェクトの設定.
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${APP_NAME})

# デバッグ実行時に参照する作業ディレクトリ.
set_target_properties(${APP_NAME} PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/Bin/${WORKSPACE_NAME}/${CMAKE_CFG_INTDIR}
)

check_variable(UI_PLUGIN_NAME)
check_variable(LOCALIZATION_PLUGIN_NAME)
check_variable(LEVEL_PLUGIN_NAME)
check_variable(ASSET_MANAGER_PLUGIN_NAME)
check_variable(ACTOR_PLUGIN_NAME)
check_variable(RENDER_PLUGIN_NAME)
check_variable(DXLIB_PLUGIN_NAME)
check_variable(ENGINE_NAME)

# ゲームアプリがリンクするライブラリ
target_link_libraries(${APP_NAME}
  ${UI_PLUGIN_NAME}
  ${LOCALIZATION_PLUGIN_NAME}
  ${LEVEL_PLUGIN_NAME}
  ${ASSET_MANAGER_PLUGIN_NAME}
  ${RENDER_PLUGIN_NAME}
  ${ACTOR_PLUGIN_NAME}
  ${DXLIB_PLUGIN_NAME}
  ${ENGINE_NAME}
)

# テストプロジェクト作成
add_subdirectory(Test)

# テストしたいライブラリを設定
target_link_libraries(${TEST_APP_NAME}
  ${UI_PLUGIN_NAME}
  ${LOCALIZATION_PLUGIN_NAME}
  ${LEVEL_PLUGIN_NAME}
  ${ASSET_MANAGER_PLUGIN_NAME}
  ${RENDER_PLUGIN_NAME}
  ${ACTOR_PLUGIN_NAME}
  ${DXLIB_PLUGIN_NAME}
  ${ENGINE_NAME}
)

