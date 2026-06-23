@echo off
title Git - Last Input
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0menu.ps1"
if errorlevel 1 (
  echo.
  echo [Script exited with an error - show this window to Claude]
  pause
)
