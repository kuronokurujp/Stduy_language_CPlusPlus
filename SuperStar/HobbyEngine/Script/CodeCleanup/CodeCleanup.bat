@echo off
setlocal

REM Clang-Formatのexeファイルパスがあるかチェック
if not defined CLANG_FORMAT_PATH (
    echo The environment variable "%CLANG_FORMAT_PATH%" does not exist.
    exit /b 1
)

REM Clang-Tidyのexeファイルパスがあるかチェック
if not defined CLANG_TIDY_PATH (
    echo The environment variable "%CLANG_TIDY_PATH%" does not exist.
    exit /b 1
)

echo The environment variable "%CLANG_FORMAT_PATH%" exists.
echo The environment variable "%CLANG_TIDY_PATH%" exists.
echo Its value is: %CLANG_FORMAT_PATH%
echo Its value is: %CLANG_TIDY_PATH%

REM プロジェクトのルートディレクトリを設定
set "PROJECT_ROOT=%~dp1"
echo Project Root Path "%PROJECT_ROOT%"

REM プロジェクトのルートディレクトリがあるか
if not exist "%PROJECT_ROOT%" (
    echo Error: Required dir "%PROJECT_ROOT%" not found.
    exit /b 1
)

REM cppおよびhファイルを再帰的に検索し、clang-formatで整形
set TEMP_FILE=temp_output.txt
for /r "%PROJECT_ROOT%" %%f in (*.cpp *.h *.hpp) do (
    echo Formatting %%f

    REM サードパーティーのファイルはコード整形非対象
    echo "%%f" | find "ThirdParty\" >NUL
    if ERRORLEVEL 1 (
        "%CLANG_FORMAT_PATH%" -i "%%f" 2> %TEMP_FILE%
        findstr /c:"error:" %TEMP_FILE% >nul
        REM 0にすると0以外の値でも条件がTRUEになってしまう
        if ERRORLEVEL 1 (
            REM clang-tidyの整形を実行
            REM "%CLANG_TIDY_PATH%" -checks='-*,modernize-concat-nested-namespaces' --fix-errors %%f
            echo "Success."
        ) else (
            echo "Error."
            type %TEMP_FILE%
        )
        del %TEMP_FILE%
    ) else (
        echo "Not Convert: ThirdParty File"
    )
)

echo All files have been formatted.

endlocal

