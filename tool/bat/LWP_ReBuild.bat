@echo off

REM Visual Studio Developer Command Promptを使用する場合
REM call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

cd "../../"

REM Debugでビルド
MSBuild "DirectXGame.sln" /t:clean;rebuild /p:ConfigurationType=StaticLibrary;Configuration=Debug;MultiProcessorCompilation=true
REM DEMOでビルド
REM MSBuild "DirectXGame.sln" /t:clean;rebuild /p:ConfigurationType=StaticLibrary;Configuration=DEMO;MultiProcessorCompilation=true
REM Releaseでビルド
MSBuild "DirectXGame.sln" /t:clean;rebuild /p:ConfigurationType=StaticLibrary;Configuration=Release;MultiProcessorCompilation=true

REM 一時ファイルを削除
rmdir "DirectXGame" /s /q

REM libを浅い階層にコピー
REM xcopy "x64/Debug/DirectXGame.lib" "../lib/LWP_Debug.lib" /y
REM xcopy "x64/Demo/DirectXGame.lib" "../lib/LWP_Demo.lib" /y
REM xcopy "x64/Release/DirectXGame.lib" "../lib/LWP_Release.lib" /y

REM システムのリソースをコピー
xcopy "resources/system" "../resources/system\" /e /y

pause