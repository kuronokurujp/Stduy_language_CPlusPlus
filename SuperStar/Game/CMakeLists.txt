# Makeに必要な情報をインストール
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICTY_SCOPE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

# Gameアプリのプロジェクト登録.
add_executable(${APP_NAME} WIN32)

# ファイル群をターゲット設定.
target_sources(${APP_NAME}
  PRIVATE
    ${TARGET_FILES}
)

# プロジェクトが参照できるインクルードディレクトリのパス.
target_include_directories(${APP_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Code)

# ランタイムライブラリを設定
target_compile_options(
  ${APP_NAME}
  PRIVATE
  $<IF:$<CONFIG:Debug>,/MTd,/MT>
)

# ビルドのマルチプロセス設定.
target_compile_options(${APP_NAME} PRIVATE /MP)
# Unicode文字セットを設定.
target_compile_options(${APP_NAME} PRIVATE /DUNICODE /D_UNICODE)

# ファイルのフィルタリング設定.
# VSプロジェクト上で見やすくするため.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TARGET_FILES})