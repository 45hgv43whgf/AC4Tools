$ErrorActionPreference = 'Stop'
$projectDir = Split-Path -Parent $PSScriptRoot
$imports = & dumpbin /nologo /imports (Join-Path $projectDir 'bin\AC4Tools.asi')
if ($LASTEXITCODE -ne 0) { throw 'Could not inspect ASI imports.' }
if (($imports -join "`n") -match '(?i)xinput[^\s]*\.dll') { throw 'ASI has a static XInput dependency.' }
$version = [Diagnostics.FileVersionInfo]::GetVersionInfo((Join-Path $projectDir 'bin\AC4Tools.asi'))
if ($version.FileVersion -ne '1.08' -or $version.FileBuildPart -ne 8) { throw 'ASI version metadata is inconsistent.' }
$fixtureRoot = Join-Path $projectDir 'obj\tests'
$gameDir = Join-Path $fixtureRoot ("install space's-" + [Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path (Join-Path $gameDir 'scripts') -Force | Out-Null
try {
    $loader = Join-Path $gameDir 'dinput8.dll'
    [IO.File]::WriteAllText($loader, 'user-supplied-loader-sentinel')
    $ini = Join-Path $gameDir 'scripts\AC4Tools.ini'
    [IO.File]::WriteAllText($ini, "[Hotkeys]`r`nMenuOpen=77`r`n[Game]`r`nTimeScale=0.25`r`n")
    & (Join-Path $projectDir 'install_to_game.ps1') -GameDir $gameDir | Out-Null
    if ([IO.File]::ReadAllText($loader) -ne 'user-supplied-loader-sentinel') { throw 'Installer replaced the user loader.' }
    $text = [IO.File]::ReadAllText($ini)
    if ($text -notmatch 'MenuOpen\s*=\s*77' -or $text -notmatch 'TimeScale\s*=\s*0.25') { throw 'INI settings were not preserved.' }
    if (-not (Test-Path -LiteralPath (Join-Path $gameDir 'scripts\AC4Tools.asi'))) { throw 'ASI was not installed.' }
    if (Test-Path -LiteralPath (Join-Path $projectDir 'release\dinput8.dll')) { throw 'Release contains an unverified loader.' }
    if (-not (Test-Path -LiteralPath (Join-Path $projectDir 'release\SHA256SUMS.txt'))) { throw 'Release checksums missing.' }
    Write-Host 'PASS: release has no loader; installer preserves loader and INI settings.'
} finally {
    # Resolve and verify the exact generated fixture before recursive cleanup.
    $resolved = (Resolve-Path -LiteralPath $gameDir).Path
    $allowed = (Resolve-Path -LiteralPath $fixtureRoot).Path + [IO.Path]::DirectorySeparatorChar
    if (-not $resolved.StartsWith($allowed, [StringComparison]::OrdinalIgnoreCase)) { throw 'Fixture escaped test root.' }
    Remove-Item -LiteralPath $resolved -Recurse -Force
}
