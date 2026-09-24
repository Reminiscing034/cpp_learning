@echo off
rem ============================================================
rem  push.bat - launcher for push.ps1 (daily commit + upload)
rem ============================================================
rem  Double-click this file to commit and upload your changes.
rem  The real logic lives in push.ps1 - open it to read the
rem  comments there.
rem
rem  ------------------------------------------------------------
rem  KEEP THIS FILE PURE ASCII. DO NOT ADD ANY NON-ASCII CHARACTER.
rem  ------------------------------------------------------------
rem  Why: cmd.exe reads .bat files using the OEM codepage
rem  (936 / GBK on this machine), but editors save files as UTF-8.
rem  Any non-ASCII byte here gets decoded into garbage, and cmd may
rem  then try to run that garbage as a command.
rem
rem  This actually happened once: a Chinese comment line got split,
rem  and its tail was executed as a command, producing the error
rem  "<mojibake> is not recognized as an internal or external
rem  command, or a batch file".
rem
rem  So: all Chinese messages belong in push.ps1, which PowerShell
rem  reads correctly (that file is saved as UTF-8 with BOM).
rem ============================================================

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0push.ps1"

rem Safety net: if PowerShell itself failed to start, keep the
rem window open so the error message can be read.
if errorlevel 1 pause
