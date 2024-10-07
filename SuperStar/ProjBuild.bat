@echo off

setlocal

rem ビルド先の絶対パスを指定.
set BUILD_DIR_FULLPATH=%~dp0
rem cmakeで実行したいCMakeLists.txtがあるディレクトリの絶対パスを指定.
set CMAKE_FILE_DIR_FULLPATH=%~dp0
rem パス連結では2重引用符はつけない.
set SCRIPT_PATH=%~dp0HobbyEngine\Script\Build\

rem プロジェクトをビルド.
call "%SCRIPT_PATH%ProjMSVC2022.bat" %BUILD_DIR_FULLPATH% %CMAKE_FILE_DIR_FULLPATH%
if %errorlevel% neq 0 pause & exit /b %errorlevel%

pause & exit /b 0

endlocal
