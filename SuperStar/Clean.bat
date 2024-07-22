@echo off
rem プロジェクトのクリーン処理

setlocal

rem  プロジェクトフォルダを削除
rmdir /s /q Build\
rem 実行ファイルを収めてフォルダを削除
rmdir /s /q Bin\

endlocal
