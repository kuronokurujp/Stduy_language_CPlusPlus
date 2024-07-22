@echo off
setlocal

REM Clang-Formatのexeファイルパスがあるかチェック
if not defined CLANG_FORMAT_PATH (
    echo The environment variable "%CLANG_FORMAT_PATH%" does not exist.
    exit /b 1
)

echo The environment variable "%CLANG_FORMAT_PATH%" exists.
echo Its value is: %CLANG_FORMAT_PATH%

REM プロジェクトのルートディレクトリを設定
set "PROJECT_ROOT=%~dp1"
echo Project Root Path "%PROJECT_ROOT%"

REM プロジェクトのルートディレクトリがあるか
if not exist "%PROJECT_ROOT%" (
    echo Error: Required dir "%PROJECT_ROOT%" not found.
    exit /b 1
)

REM cppおよびhファイルを再帰的に検索し、clang-formatで整形
for /r "%PROJECT_ROOT%" %%f in (*.cpp *.h *.hpp) do (
    echo Formatting %%f
    "%CLANG_FORMAT_PATH%" -i "%%f"
)

echo All files have been formatted.
endlocal