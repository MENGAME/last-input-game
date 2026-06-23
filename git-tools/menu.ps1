$ErrorActionPreference = "Stop"
[Console]::OutputEncoding = [Text.Encoding]::UTF8

# Репозиторий = папка на уровень выше (git-tools лежит внутри проекта)
$RepoRoot = Split-Path $PSScriptRoot -Parent
Set-Location $RepoRoot

# --- найти git (он установлен, но может быть не в PATH) ---
$git = "git"
if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    $fallback = "C:\Program Files\Git\cmd\git.exe"
    if (Test-Path $fallback) { $git = $fallback }
    else {
        Write-Host "git не найден. Установи Git for Windows." -ForegroundColor Red
        Read-Host "Enter для выхода"; exit 1
    }
}

function Wait-Key { Write-Host ""; Read-Host "Enter — вернуться в меню" | Out-Null }

function Do-Push {
    Write-Host "=== PUSH: отправить мои изменения на GitHub ===" -ForegroundColor Cyan; Write-Host ""
    & $git add -A
    & $git diff --cached --quiet
    if ($LASTEXITCODE -eq 0) { Write-Host "Нет изменений — всё уже запушено." -ForegroundColor Yellow; return }
    Write-Host "Изменения:" -ForegroundColor Cyan; & $git status -s; Write-Host ""
    $msg = Read-Host "Описание изменений"
    if ([string]::IsNullOrWhiteSpace($msg)) { $msg = "update" }
    & $git commit -m $msg
    & $git push
    if ($LASTEXITCODE -ne 0) { Write-Host ""; Write-Host "Push не прошёл (см. выше). Коммит сохранён локально." -ForegroundColor Red; return }
    Write-Host ""; Write-Host "Готово — изменения на GitHub." -ForegroundColor Green
}

function Do-Pull {
    Write-Host "=== PULL: забрать изменения с GitHub ===" -ForegroundColor Cyan; Write-Host ""
    & $git pull --no-rebase
    if ($LASTEXITCODE -ne 0) { Write-Host ""; Write-Host "Pull не прошёл — конфликт или незакоммиченные правки. Покажи мне вывод выше." -ForegroundColor Red; return }
    Write-Host ""; Write-Host "Готово — локальная копия обновлена." -ForegroundColor Green
}

function Do-Sync {
    Write-Host "=== SYNC: забрать чужое + отправить своё ===" -ForegroundColor Cyan; Write-Host ""
    & $git add -A
    & $git diff --cached --quiet
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Мои изменения:" -ForegroundColor Cyan; & $git status -s; Write-Host ""
        $msg = Read-Host "Описание изменений"
        if ([string]::IsNullOrWhiteSpace($msg)) { $msg = "update" }
        & $git commit -m $msg
    } else { Write-Host "Своих изменений нет — только забираю чужое." -ForegroundColor DarkGray }
    Write-Host ""; Write-Host "Забираю с GitHub..." -ForegroundColor Cyan
    & $git pull --no-rebase
    if ($LASTEXITCODE -ne 0) { Write-Host ""; Write-Host "Pull не прошёл (конфликт?). Покажи мне вывод — помогу разрулить." -ForegroundColor Red; return }
    Write-Host ""; Write-Host "Отправляю на GitHub..." -ForegroundColor Cyan
    & $git push
    if ($LASTEXITCODE -ne 0) { Write-Host ""; Write-Host "Push не прошёл (см. выше). Коммит сохранён локально." -ForegroundColor Red; return }
    Write-Host ""; Write-Host "Готово — синхронизировано в обе стороны." -ForegroundColor Green
}

function Do-Status {
    Write-Host "=== STATUS: что изменилось и насколько отстал от GitHub ===" -ForegroundColor Cyan; Write-Host ""
    & $git fetch | Out-Null
    & $git status
}

function Do-Undo {
    Write-Host "=== UNDO: отменить последний коммит (файлы сохранятся) ===" -ForegroundColor Yellow; Write-Host ""
    Write-Host "Последний коммит:" -ForegroundColor Cyan; & $git log -1 --oneline; Write-Host ""
    Write-Host "ВНИМАНИЕ: если он уже запушен на GitHub — не отменяй, копии разойдутся. Тогда спроси меня." -ForegroundColor Red; Write-Host ""
    $ans = Read-Host "Отменить последний коммит? (y/n)"
    if ($ans -ne "y") { Write-Host "Отменено, ничего не трогал." -ForegroundColor DarkGray; return }
    & $git reset --soft HEAD~1
    Write-Host ""; Write-Host "Готово — коммит снят, изменения вернулись в неотправленные." -ForegroundColor Green
}

# --- цикл меню ---
while ($true) {
    Clear-Host
    Write-Host "============================================" -ForegroundColor DarkCyan
    Write-Host "        GIT — Last Input" -ForegroundColor White
    Write-Host "============================================" -ForegroundColor DarkCyan
    Write-Host ""
    Write-Host "  1" -ForegroundColor Green -NoNewline; Write-Host "  SYNC    " -ForegroundColor White -NoNewline; Write-Host "забрать чужое + отправить своё (на каждый день)" -ForegroundColor Gray
    Write-Host "  2" -ForegroundColor Green -NoNewline; Write-Host "  PULL    " -ForegroundColor White -NoNewline; Write-Host "забрать с GitHub (жми перед началом работы)" -ForegroundColor Gray
    Write-Host "  3" -ForegroundColor Green -NoNewline; Write-Host "  PUSH    " -ForegroundColor White -NoNewline; Write-Host "только отправить своё" -ForegroundColor Gray
    Write-Host "  4" -ForegroundColor Green -NoNewline; Write-Host "  STATUS  " -ForegroundColor White -NoNewline; Write-Host "посмотреть, что изменилось (ничего не меняет)" -ForegroundColor Gray
    Write-Host "  5" -ForegroundColor Green -NoNewline; Write-Host "  UNDO    " -ForegroundColor White -NoNewline; Write-Host "отменить последний коммит (файлы сохранятся)" -ForegroundColor Gray
    Write-Host ""
    Write-Host "  0" -ForegroundColor DarkGray -NoNewline; Write-Host "  Выход" -ForegroundColor DarkGray
    Write-Host ""
    Write-Host "--------------------------------------------" -ForegroundColor DarkCyan
    $choice = Read-Host "Выбери пункт"
    Write-Host ""
    switch ($choice) {
        "1" { Do-Sync;   Wait-Key }
        "2" { Do-Pull;   Wait-Key }
        "3" { Do-Push;   Wait-Key }
        "4" { Do-Status; Wait-Key }
        "5" { Do-Undo;   Wait-Key }
        "0" { exit 0 }
        default { Write-Host "Нет такого пункта — введи цифру 0-5." -ForegroundColor Yellow; Start-Sleep -Milliseconds 900 }
    }
}
