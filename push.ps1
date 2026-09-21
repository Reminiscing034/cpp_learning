# ============================================================
#  push.ps1 —— 一键提交并上传到 GitHub
# ============================================================
#  用法（三选一）：
#    1. 双击同目录下的 push.bat          ← 最简单，推荐
#    2. 在本目录开 PowerShell，敲： .\push.ps1
#    3. 直接给说明（不弹输入）：      .\push.ps1 -Message "week1 完成两道题"
#
#  可选参数：
#    -Message "..."   直接指定提交说明，不再询问
#    -NoPush          只提交到本地，不推送到 GitHub
#
#  注意：本文件必须以 UTF-8 带 BOM 保存，
#        否则 Windows PowerShell 5.1 读中文会解析报错。
# ============================================================

[CmdletBinding()]
param(
    [string]$Message,
    [switch]$NoPush
)

# 不要用 'Stop' —— 原生命令（git）往 stderr 写提示时会被误当成致命错误。
# 下面所有关键步骤都显式检查 $LASTEXITCODE。
$ErrorActionPreference = 'Continue'

# 让 git 输出里的中文文件名正常显示
try { [Console]::OutputEncoding = [System.Text.Encoding]::UTF8 } catch { }

function Title([string]$t) {
    Write-Host ''
    Write-Host "  == $t" -ForegroundColor Cyan
    Write-Host '  ------------------------------------------------------------' -ForegroundColor DarkGray
}
function Info([string]$m)  { Write-Host "  $m" -ForegroundColor Gray }
function Ok([string]$m)    { Write-Host "  $m" -ForegroundColor Green }
function Warn([string]$m)  { Write-Host "  $m" -ForegroundColor Yellow }
function Fail([string]$m)  { Write-Host "  $m" -ForegroundColor Red }

function Finish([int]$code) {
    # 只有在交互式窗口里才等按键；被别的脚本调用时不阻塞
    if (-not [Console]::IsInputRedirected) {
        Write-Host ''
        Write-Host '  按回车键关闭这个窗口...' -ForegroundColor DarkGray
        $null = Read-Host
    }
    exit $code
}

# ------------------------------------------------------------
# 1. 定位仓库根目录
# ------------------------------------------------------------
$root = $null

# 先按「当前所在目录」找仓库
$try = & git rev-parse --show-toplevel 2>$null
if ($LASTEXITCODE -eq 0 -and $try) { $root = $try }

# 找不到就退回「脚本自己所在的目录」——双击运行时更可靠
if (-not $root -and $PSScriptRoot) {
    $try = & git -C $PSScriptRoot rev-parse --show-toplevel 2>$null
    if ($LASTEXITCODE -eq 0 -and $try) { $root = $try }
}

if (-not $root) {
    Write-Host ''
    Fail '这里不是 git 仓库。'
    Info '请把 push.ps1 / push.bat 放在仓库文件夹（D:\code\cpp_learning）里再运行。'
    Finish 1
}
$root = ($root -replace '/', '\').Trim()
Set-Location -LiteralPath $root

Write-Host ''
Write-Host '  ==============================================================' -ForegroundColor Green
Write-Host '            一键提交并上传到 GitHub' -ForegroundColor Green
Write-Host '  ==============================================================' -ForegroundColor Green
Info "仓库位置：$root"

# ------------------------------------------------------------
# 2. 看看有哪些改动
# ------------------------------------------------------------
Title '第 1 步 / 5：检查改动'

$status = & git status --porcelain
$changedCount = 0
if ($status) { $changedCount = ($status | Measure-Object).Count }

# 有没有「已提交但还没推送」的
$ahead = 0
& git rev-parse --abbrev-ref '@{u}' 2>$null | Out-Null
if ($LASTEXITCODE -eq 0) {
    $ahead = [int](& git rev-list --count '@{u}..HEAD' 2>$null)
}

if ($changedCount -eq 0) {
    Ok '没有需要提交的改动 —— 文件都是最新的。'
    if ($ahead -gt 0) {
        Warn "但有 $ahead 个本地提交还没推送到 GitHub。"
    } else {
        Info '（GitHub 上也已经同步，什么都不用做）'
        Finish 0
    }
} else {
    & git status --short
    Write-Host ''
    Warn "共 $changedCount 处改动。"
}

# ------------------------------------------------------------
# 3. 安全检查：别把编译产物提交上去
# ------------------------------------------------------------
Title '第 2 步 / 5：安全检查'

# 只揪「会被加进仓库」的编译产物。
# 注意：状态里带 D 的是「从仓库移除」，那是好事，不能报警。
$badPattern = '\.vs/|/x64/|/Debug/|/Release/|\.vcxproj\.user$|\.exe$|\.obj$|\.pdb$|\.tlog$|\.ilk$|\.idb$'

$dirty = & git status --porcelain | Where-Object {
    $code = if ($_.Length -ge 2) { $_.Substring(0, 2) } else { '' }
    if ($code -match 'D') { $false }                       # 删除不算问题
    else { $_.Substring(3) -match $badPattern }
}

if ($dirty) {
    Warn '发现可能不该提交的文件：'
    $dirty | ForEach-Object { Write-Host "     $_" -ForegroundColor Yellow }
    Write-Host ''
    Warn '这些看起来是编译产物，正常应该被 .gitignore 挡掉。'
    if ([Console]::IsInputRedirected) {
        Fail '（非交互模式，无法询问，已取消）'
        Finish 1
    }
    $ans = Read-Host '  还是要继续吗？(输入 y 继续，其他键取消)'
    if ($ans -notmatch '^[Yy]') {
        Fail '已取消，什么都没做。'
        Finish 1
    }
} else {
    Ok '干净 —— 没有编译产物混进来。'
}

# ------------------------------------------------------------
# 4. 提交说明
# ------------------------------------------------------------
Title '第 3 步 / 5：写一句说明'

if (-not $Message) {
    if ([Console]::IsInputRedirected) {
        Fail '非交互模式下必须用 -Message 指定提交说明。'
        Finish 1
    }
    Info '简单说说这次做了什么。这句话会显示在 GitHub 上，'
    Info '以后回头看就靠它认路了。'
    Write-Host ''
    Write-Host '  例如：week1 完成括号匹配和约瑟夫环' -ForegroundColor DarkGray
    Write-Host ''
    $Message = Read-Host '  说明'
}

$Message = "$Message".Trim()
if (-not $Message) {
    Write-Host ''
    Fail '说明不能为空 —— 空说明以后自己也看不懂改了什么。'
    Fail '已取消，没有做任何提交。'
    Finish 1
}

# ------------------------------------------------------------
# 5. 提交
# ------------------------------------------------------------
Title '第 4 步 / 5：提交到本地'

& git add -A
if ($LASTEXITCODE -ne 0) { Fail 'git add 失败。'; Finish 1 }

& git commit -m $Message
if ($LASTEXITCODE -ne 0) {
    Fail 'git commit 失败。'
    Finish 1
}
Ok '已生成一个本地提交。'

# ------------------------------------------------------------
# 6. 推送
# ------------------------------------------------------------
if ($NoPush) {
    Write-Host ''
    Warn '按你的要求，只提交到本地，没有推送。'
    Info '想推送时，再运行一次本脚本即可。'
    Finish 0
}

Title '第 5 步 / 5：推送到 GitHub'

$branch = (& git branch --show-current).Trim()
& git rev-parse --abbrev-ref '@{u}' 2>$null | Out-Null
$hasUpstream = ($LASTEXITCODE -eq 0)

if ($hasUpstream) {
    Info "推送到 origin/$branch ..."
    & git push
} else {
    Info "分支 $branch 还没有关联远程，正在建立关联并推送 ..."
    & git push -u origin $branch
}

if ($LASTEXITCODE -ne 0) {
    Write-Host ''
    Fail '推送失败。常见原因：'
    Warn '  · 网络问题 —— 换个网络，或稍后重试'
    Warn '  · 登录过期 —— 再运行一次，在弹出的窗口里重新登录 GitHub'
    Warn '  · 远程有本地没有的提交 —— 比如你在 GitHub 网页上直接改过文件'
    Write-Host ''
    Ok '别担心：你的提交已经安全保存在本地了，不会丢。'
    Info '问题解决后再运行一次本脚本就行。'
    Finish 1
}

Write-Host ''
Write-Host '  ==============================================================' -ForegroundColor Green
Write-Host '        全部完成！代码已经上传到 GitHub' -ForegroundColor Green
Write-Host '  ==============================================================' -ForegroundColor Green
Info '去 https://github.com/Reminiscing034/cpp_learning 看看就能看到。'
Finish 0
