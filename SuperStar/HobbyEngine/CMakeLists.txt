# 共通情報を取得.
include(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake NO_POLICTY_SCOPE)

message("build engine lib!")

set(CACHE_ENGINE_INCLUDE_CODE ${CMAKE_CURRENT_SOURCE_DIR}/Code CACHE STRING global_engine_include_code)

# 共有ライブラリとして生成するためのオプションの定義.
option(ENGINE_BUILD_SHARED_LIBS "build engine as a shared library" OFF)

# オプションで生成するライブラリの種類(SHARED/STATIC)を切り替える.
if (ENGINE_BUILD_SHARED_LIBS)
    add_library(${ENGINE_NAME} SHARED)
else()
    add_library(${ENGINE_NAME} STATIC)
endif()

# C++のコンパイラの設定
set_target_properties(${ENGINE_NAME} PROPERTIES
	CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# engine生成に必要なソースファイル.
target_sources(${ENGINE_NAME}
    PRIVATE
    ${ENGINE_TARGET_FILES}
)

# Windows特有のライブラリをリンク
if (MSVC)
    target_link_libraries(${ENGINE_NAME} PRIVATE PathCch)
endif()

# プロジェクトが参照できるインクルードディレクトリのパス.
target_include_directories(${ENGINE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Code)

# ランタイムライブラリを設定
target_compile_options(
  ${ENGINE_NAME}
  PRIVATE
  $<IF:$<CONFIG:Debug>,/MTd,/MT>
)

# ビルドのマルチプロセス設定.
target_compile_options(${ENGINE_NAME} PRIVATE /MP)
# Unicode文字セットを設定.
target_compile_options(${ENGINE_NAME} PRIVATE /DUNICODE /D_UNICODE)

# ファイルのフィルタリング設定.
# VSプロジェクト上で見やすくするため.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINE_TARGET_FILES})