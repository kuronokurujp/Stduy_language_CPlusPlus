@echo off
chcp 65001

setlocal enabledelayedexpansion
echo "start copy asset process"

set SRC_DIR_PATH=%1
set DST_DIR_PATH=%2

echo "src dir path"
echo %SRC_DIR_PATH%

echo "dst dir path"
echo %DST_DIR_PATH%

echo "del dir path"
echo %DST_DIR_PATH%
rmdir %DST_DIR_PATH% /s /q

xcopy %SRC_DIR_PATH% %DST_DIR_PATH%  /E /I

echo "end"
endlocal
