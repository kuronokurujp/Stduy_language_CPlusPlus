@echo off

setlocal

rem ~をつけると二重引用符が外れて変数に代入される.
set MSVC_VERSION=%~2
set MSVC_NUMBER=%~3
set VCVARSALL=%~4
set GENERATOR=%~1
set CMAKE_FILE_ROOT_DIR_FULLPATH=%~5
set BUILD_DIR_FULLPATH=%~6

rem 変数チェック.
set VARIABLE_EXISTS=TRUE
if "%MSVC_VERSION%" equ "" set VARIABLE_EXISTS=FALSE
if "%MSVC_NUMBER%" equ "" set VARIABLE_EXISTS=FALSE
if "%VCVARSALL%" equ "" set VARIABLE_EXISTS=FALSE
if "%GENERATOR%" equ "" set VARIABLE_EXISTS=FALSE
if "%CMAKE_FILE_ROOT_DIR_FULLPATH%" equ "" set VARIABLE_EXISTS=FALSE
if "%BUILD_DIR_FULLPATH%" equ "" set VARIABLE_EXISTS=FALSE
if "%VARIABLE_EXISTS%" equ "FALSE" (
  echo [GameProject] Invalid batch call.
  exit /b 1
)

rem VCが用意したビルド環境構築バッチファイルがあるかチェック.
if not exist "%VCVARSALL%" (
  echo [GameProject] Visual C++ Compiler %MSVC_VERSION% not found.
  exit /b 1
)

rem ----------------
rem ユーザー入力開始.
rem ----------------

rem アーキテクチャ入力.
echo.
echo Select which architecture to use.
echo **************************************************
echo 1. x86 (Win32)
echo 2. x64 (Win64)
echo.
choice /c:12 /n /m ">"
set SELECTED=%errorlevel%
if "%SELECTED%" equ "1" (
  set ARCHITECTURE=x86
  set CMAKE_A_OPTION=Win32
) else if "%SELECTED%" equ "2" (
  set ARCHITECTURE=x64
  set CMAKE_A_OPTION=x64
) else (
  echo [GameProject] Invalid option.
  exit /b 1
)

rem ランタイム入力.
echo.
echo Select which cubism core c runtime library to use
echo **************************************************
echo 1. use the multithread-specific and DLL-specific version of the run-time library (MD)
echo 2. use the multithread, static version of the run-time library (MT)
echo.
choice /c:12 /n /m ">"
set SELECTED=%errorlevel%
if "%SELECTED%" equ "1" (
  set CORE_CRL_MD=ON
) else if "%SELECTED%" equ "2" (
  set CORE_CRL_MD=OFF
) else (
  echo [GameProject] Invalid option.
  exit /b 1
)

rem ------
rem ビルド.
rem ------

echo Building %ARCHITECTURE% with Visual C++ Compiler %MSVC_VERSION%...

rem 指定したビルドアーキテクチャからvcvarsall.batでプロジェクトを作成するビルド環境構築.
call "%VCVARSALL%" %ARCHITECTURE%
if %errorlevel% neq 0 exit /b %errorlevel%

rem ランタイムライブラリのタイプを設定.
rem md: マルチスレッドのdll対応.
rem mt: マルチスレッドのlib対応.
if "%CORE_CRL_MD%" equ "ON" (
  set CORE_CRL=md
) else (
  set CORE_CRL=mt
)

rem ビルドを出力するディレクトリパス.
set BUILD_PATH=%BUILD_DIR_FULLPATH%\Build\%GENERATOR%MSVC%MSVC_VERSION%_%ARCHITECTURE%_%CORE_CRL%

rem cmakeを実行するためにCMakeLists.txtがあるディレクトリに移動.
cd %CMAKE_FILE_ROOT_DIR_FULLPATH%

rem CMakeコマンドを実行してMSVCプロジェクトを生成.
cmake -S . -B "%BUILD_PATH%" ^
  -G "Visual Studio %MSVC_NUMBER% %MSVC_VERSION%" ^
  -A %CMAKE_A_OPTION% ^
  -D CORE_CRL_MD=%CORE_CRL_MD%
if %errorlevel% neq 0 exit /b %errorlevel%

exit /b 0

endlocal