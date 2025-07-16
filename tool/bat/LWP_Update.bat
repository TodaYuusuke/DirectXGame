@echo off

REM Visual Studio Developer Command Promptを使用する場合
REM call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

cd "../../"

REM 変更をすべて取り消す
git reset --hard
REM Gitで最新のコードを取得
git pull origin master
if ERRORLEVEL 1 (
    REM ここにエラー発生時の処理を記述します
    echo.
    echo git command could not be executed.
    echo Please check if git is installed.
    echo Please ask your administrator for error details.
    echo.
    pause
    exit /b
)

REM Debugでビルド
MSBuild "DirectXGame.vcxproj" /t:clean;rebuild /p:ConfigurationType=StaticLibrary;Configuration=Debug;Platform=x64;MultiProcessorCompilation=true
REM Releaseでビルド
MSBuild "DirectXGame.vcxproj" /t:clean;rebuild /p:ConfigurationType=StaticLibrary;Configuration=Release;Platform=x64;MultiProcessorCompilation=true

REM 一時ファイルを削除
rmdir "DirectXGame" /s /q

REM システムのリソースをコピー
xcopy "SampleGame/resources/system" "../resources/system\" /e /y

pause