@echo off
rem ============================================================
rem  push.bat —— 双击这个文件，就能提交并上传到 GitHub
rem ============================================================
rem  它只是把同目录下的 push.ps1 跑起来。
rem  真正的逻辑都在 push.ps1 里，看不懂可以打开那个文件看注释。
rem ============================================================

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0push.ps1"

rem 如果 PowerShell 自己没能暂停（比如启动就失败），这里兜个底
if errorlevel 1 pause
