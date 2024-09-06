@echo off
chcp 65001

setlocal enabledelayedexpansion

REM ----------------
REM ユーザー入力開始.
REM ----------------

REM ファイル名入力.
:input_filename_loop
set /p INPUT_FILENAME="Please enter a file name: "
REM 入力が空であれば再入力を促す
if "%INPUT_FILENAME%"=="" (
    echo Input cannot be empty. Please try again.
    goto input_filename_loop
)

:input_namespace_name_loop
set /p INPUT_NAMESPACE_NAME="Please enter a namespace name: "
REM 入力が空であれば再入力を促す
if "%INPUT_NAMESPACE_NAME%"=="" (
    echo Input cannot be empty. Please try again.
    goto input_namespace_name_loop
)

REM 元のファイル名と新しいファイル名を設定します
set "SOURCE_FILE=Template\Template.h"
set "DESTINATION_FILE=%INPUT_FILENAME%.h"

REM ファイルをコピーします
copy "%SOURCE_FILE%" "%DESTINATION_FILE%"

REM コピーしたファイルを一時ファイルとして処理
set "TEMP_FILE=%DESTINATION_FILE%.tmp"

REM テンプレート名の置換
(for /f "usebackq delims=" %%A in ("%DESTINATION_FILE%") do (
    set "line=%%A"
    setlocal enabledelayedexpansion
    echo !line:__TEMPLATE_NAME__=%INPUT_NAMESPACE_NAME%!>> "%TEMP_FILE%"
    endlocal
)) 

REM 一時ファイルを新しいファイルとして置き換え
move /y "%TEMP_FILE%" "%DESTINATION_FILE%"

REM 元のファイル名と新しいファイル名を設定します
set "SOURCE_FILE=Template\Template.cpp"
set "DESTINATION_FILE=%INPUT_FILENAME%.cpp"

REM ファイルをコピーします
copy "%SOURCE_FILE%" "%DESTINATION_FILE%"

REM コピーしたファイルを一時ファイルとして処理
set "TEMP_FILE=%DESTINATION_FILE%.tmp"

REM テンプレート名の置換
(for /f "usebackq delims=" %%A in ("%DESTINATION_FILE%") do (
    set "line=%%A"
    setlocal enabledelayedexpansion
    set "line=!line:__INC_FILENAME__=%INPUT_FILENAME%!"
    set "line=!line:__TEMPLATE_NAME__=%INPUT_NAMESPACE_NAME%!"
    echo !line!>> "%TEMP_FILE%"
    endlocal
)) 

REM 一時ファイルを新しいファイルとして置き換え
move /y "%TEMP_FILE%" "%DESTINATION_FILE%"


echo 書き換えが完了しました: "%DESTINATION_FILE%"
pause
exit /b 0

endlocal