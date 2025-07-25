@echo off
setlocal enabledelayedexpansion

REM Visual Studio Developer Command Prompt を使用する（環境に応じて調整）
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

cd "../../"

REM ブランチ名を入力させる
set /p branchName=チェックアウトするブランチ名を入力してください（例: feature/new-ui）:

REM 入力が空ならエラー扱い
if "%branchName%"=="" (
    echo.
    echo ブランチ名が入力されていません。
    pause
    exit /b
)

REM 変更をすべて取り消す
git reset --hard

REM 指定したブランチにチェックアウト
git checkout %branchName%
if ERRORLEVEL 1 (
    echo.
    echo ブランチ %branchName% のチェックアウトに失敗しました。
    pause
    exit /b
)

REM 指定したブランチをプル
git pull origin %branchName%
if ERRORLEVEL 1 (
    echo.
    echo git pull に失敗しました。ブランチ名やネットワークを確認してください。
    pause
    exit /b
)

REM Debug ビルド
MSBuild "DirectXGame.vcxproj" /t:clean;rebuild /p:ConfigurationType=StaticLibrary;Configuration=Debug;Platform=x64;MultiProcessorCompilation=true

REM Release ビルド
MSBuild "DirectXGame.vcxproj" /t:clean;rebuild /p:ConfigurationType=StaticLibrary;Configuration=Release;Platform=x64;MultiProcessorCompilation=true

REM ビルド後に不要なディレクトリを削除
rmdir "DirectXGame" /s /q

REM リソースをコピー
xcopy "SampleGame/resources/system" "../resources/system\" /e /y

pause