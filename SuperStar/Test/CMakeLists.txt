# Makeに必要な情報をインストール
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICTY_SCOPE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

# 単体テストアプリのプロジェクト登録.
add_executable(${TEST_APP_NAME})

set_property(TARGET ${TEST_APP_NAME} PROPERTY WIN32_EXECUTABLE false)

# ファイル群をターゲット設定.
target_sources(${TEST_APP_NAME}
  PRIVATE
    ${TEST_TARGET_FILES}
)

# プロジェクトが参照できるインクルードディレクトリのパス.
target_include_directories(${TEST_APP_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# ランタイムライブラリを設定
target_compile_options(
  ${TEST_APP_NAME}
  PRIVATE
  $<IF:$<CONFIG:Debug>,/MTd,/MT>
)

# ビルドのマルチプロセス設定.
target_compile_options(${TEST_APP_NAME} PRIVATE /MP)
# Unicode文字セットを設定.
target_compile_options(${TEST_APP_NAME} PRIVATE /DUNICODE /D_UNICODE)

# ファイルのフィルタリング設定.
# VSプロジェクト上で見やすくするため.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TEST_TARGET_FILES})