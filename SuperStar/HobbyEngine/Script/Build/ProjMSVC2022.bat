@echo off

setlocal

rem CMakeで作成したBuildのルートディレクトリの絶対パス.
set BUILD_DIR_FULLPATH=%1
rem CMakeLists.txtがあるディレクトリの絶対パス.
set CMAKE_FILE_DIR_FULLPATH=%2

rem 変数が空かどうかチェック.
set VARIABLE_EXISTS=TRUE
if "%BUILD_DIR_FULLPATH%" equ "" set VARIABLE_EXISTS=FALSE
if "%CMAKE_FILE_DIR_FULLPATH%" equ "" set VARIABLE_EXISTS=FALSE
if "%VARIABLE_EXISTS%" equ "FALSE" (
  echo [ProjMSVC2022] Invalid batch call.
  exit /b 1
)

rem このバッチファイルの絶対パスを取得.
set SCRIPT_PATH=%~dp0
rem VS2022のバージョン情報を設定.
set MSVC_VERSION=2022
set MSVC_NUMBER=17
rem VSインストーラーで取得したコマンドラインビルドツールパスを取得.
set VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe
set /a _MSVC_RANGE_S=MSVC_NUMBER
set /a _MSVC_RANGE_E=MSVC_NUMBER+1
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -legacy -version [%_MSVC_RANGE_S%^,%_MSVC_RANGE_E%^) -property installationPath`) do (
  set VCVARSALL="%%i\VC\Auxiliary\Build\vcvarsall.bat"
)
rem cmakeで生成する種類を設定.
set GENERATOR=Proj

rem VS2022のプロジェクトを生成.
call %SCRIPT_PATH%MSVCCommon.bat %GENERATOR% %MSVC_VERSION% %MSVC_NUMBER% %VCVARSALL% %BUILD_DIR_FULLPATH% %CMAKE_FILE_DIR_FULLPATH%
if %errorlevel% neq 0 pause & exit /b %errorlevel%

endlocal
