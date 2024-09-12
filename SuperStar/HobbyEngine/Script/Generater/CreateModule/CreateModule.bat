@echo off
chcp 65001

setlocal enabledelayedexpansion

REM ----------------
REM ユーザー入力開始.
REM ----------------

REM モジュール名入力.
:input_modulename_loop
set /p INPUT_MODULENAME="Please enter a module name: "
REM 入力が空であれば再入力を促す
if "%INPUT_MODULENAME%"=="" (
    echo Input cannot be empty. Please try again.
    goto input_modulename_loop
)

REM 作成したモジュールの転送先
:input_module_dir_fullpath_loop
set /p INPUT_MODULE_DIR_FULLPATH="Please enter a module transform dir fullpath: "
REM 入力が空であれば再入力を促す
if "%INPUT_MODULE_DIR_FULLPATH%"=="" (
    echo Input cannot be empty. Please try again.
    goto input_module_dir_fullpath_loop
)

echo 用意したテンプレートモジュールをコピーします
set "DEST_MODULEDIR=%INPUT_MODULENAME%"
REM テンプレートをコピーしてフォルダ名をモジュール名にする
robocopy "Template" "%DEST_MODULEDIR%" /E

REM 大文字のモジュール名を作成
set "MODULE_UPPER_NAME="

REM 文字列の長さまでループさせる
for /l %%i in (0,1,25) do (
    set "lower=abcdefghijklmnopqrstuvwxyz"
    set "upper=ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    set "char=!INPUT_MODULENAME:~%%i,1!"
    
    REM 文字列の終端に到達した場合ループを終了
    if "!char!" == "" goto done

    for /l %%j in (0,1,25) do (
        if "!char!"=="!lower:~%%j,1!" set "char=!upper:~%%j,1!"
    )

    set "MODULE_UPPER_NAME=!MODULE_UPPER_NAME!!char!"
)

:done
echo 大文字モジュール名は: !MODULE_UPPER_NAME!

echo コピーしたモジュールを入力情報に基づいて書き換えます
set ORG_MODULE_CPP_FILENAME=Module.cpp
set ORG_MODULE_H_FILENAME=Module.h
set REPLACE_MODULE_CPP_FILENAME=%INPUT_MODULENAME%Module.cpp
set REPLACE_MODULE_H_FILENAME=%INPUT_MODULENAME%Module.h

REM 置換キーワードが記述しているファイル一覧
set REPLACE_FILES[0]=README.md
set REPLACE_FILES[1]=%ORG_MODULE_CPP_FILENAME%
set REPLACE_FILES[2]=%ORG_MODULE_H_FILENAME%
set REPLACE_FILES[3]=Config.cmake
set REPLACE_FILES[4]=CMakeLists.txt

set ARRAY_SIZE=5

REM 一時ファイルを作成
set TEMP_FILE=%TEMP%\replace_temp.txt

REM ファイル内の指定キーワードを置換
for /L %%i in (0,1,%ARRAY_SIZE%) do (
    set "CURRENT_FILE=!REPLACE_FILES[%%i]!"
    if exist "!DEST_MODULEDIR!\!CURRENT_FILE!" (
        (for /f "usebackq delims=" %%A in ("!DEST_MODULEDIR!\!CURRENT_FILE!") do (
            set "line=%%A"
            setlocal enabledelayedexpansion
            set "line=!line:{MODULE_NAME}=%INPUT_MODULENAME%!"
            set "line=!line:{MODULE_UPPER_NAME}=%MODULE_UPPER_NAME%!"
            echo !line!>> "%TEMP_FILE%"
            endlocal
        )) 
        move /Y "%TEMP_FILE%" "!DEST_MODULEDIR!\!CURRENT_FILE!"
    ) else (
        echo File not found: !CURRENT_FILE!
    )
)

echo モジュールのファイル名を入力した名前に基づいて変える
rename "%DEST_MODULEDIR%\%ORG_MODULE_CPP_FILENAME%" "%REPLACE_MODULE_CPP_FILENAME%"
rename "%DEST_MODULEDIR%\%ORG_MODULE_H_FILENAME%" "%REPLACE_MODULE_H_FILENAME%"

echo コピーしたモジュールを指定したフォルダに移動します。
move "%DEST_MODULEDIR%" "%INPUT_MODULE_DIR_FULLPATH%"

echo 新規モジュール作成が成功しました
pause
exit /b 0

:end
echo 新規モジュール作成に失敗しました
pause
exit /b -1

endlocal
