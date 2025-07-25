@echo off
setlocal enabledelayedexpansion

REM Visual Studio Developer Command Prompt ���g�p����i���ɉ����Ē����j
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

cd "../../"

REM �u�����`������͂�����
set /p branchName=�`�F�b�N�A�E�g����u�����`������͂��Ă��������i��: feature/new-ui�j:

REM ���͂���Ȃ�G���[����
if "%branchName%"=="" (
    echo.
    echo �u�����`�������͂���Ă��܂���B
    pause
    exit /b
)

REM �ύX�����ׂĎ�����
git reset --hard

REM �w�肵���u�����`�Ƀ`�F�b�N�A�E�g
git checkout %branchName%
if ERRORLEVEL 1 (
    echo.
    echo �u�����` %branchName% �̃`�F�b�N�A�E�g�Ɏ��s���܂����B
    pause
    exit /b
)

REM �w�肵���u�����`���v��
git pull origin %branchName%
if ERRORLEVEL 1 (
    echo.
    echo git pull �Ɏ��s���܂����B�u�����`����l�b�g���[�N���m�F���Ă��������B
    pause
    exit /b
)

REM Debug �r���h
MSBuild "DirectXGame.vcxproj" /t:clean;rebuild /p:ConfigurationType=StaticLibrary;Configuration=Debug;Platform=x64;MultiProcessorCompilation=true

REM Release �r���h
MSBuild "DirectXGame.vcxproj" /t:clean;rebuild /p:ConfigurationType=StaticLibrary;Configuration=Release;Platform=x64;MultiProcessorCompilation=true

REM �r���h��ɕs�v�ȃf�B���N�g�����폜
rmdir "DirectXGame" /s /q

REM ���\�[�X���R�s�[
xcopy "SampleGame/resources/system" "../resources/system\" /e /y

pause