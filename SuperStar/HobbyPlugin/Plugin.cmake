# プラグインの共通処理を記述

# プラグインの共通ビルド処理
function(common_build_plugin name files engine_includes bShared)
    # オプションで生成するライブラリの種類(SHARED/STATIC)を切り替える.
    if (${bShared})
        add_library(${name} SHARED)
    else()
        add_library(${name} STATIC)
    endif()

    # C++のコンパイラの設定
    set_target_properties(${name} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )

    # 生成に必要なソースファイル.
    target_sources(${name}
        PRIVATE
        ${files}
    )

    # プロジェクトが参照できるインクルードディレクトリのパス.
    # リンク側は参照してほしくない
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/)
    # リンク側で参照してほしい
    target_include_directories(${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

    target_include_directories(${name}
        PRIVATE
            # Engineのインクルードディレクトリ.
            ${engine_includes}
    )

    # ランタイムライブラリを設定
    target_compile_options(
        ${name}
        PRIVATE
        $<IF:$<CONFIG:Debug>,/MTd,/MT>
    )

    # ビルドのマルチプロセス設定.
    target_compile_options(${name} PRIVATE /MP)
    # Unicode文字セットを設定.
    target_compile_options(${name} PRIVATE /DUNICODE /D_UNICODE)

    # ファイルのフィルタリング設定.
    # VSプロジェクト上で見やすくするため.
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${files})
endfunction()