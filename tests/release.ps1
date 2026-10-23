$ErrorActionPreference = 'Stop'
$projectDir = Split-Path -Parent $PSScriptRoot
$imports = & dumpbin /nologo /imports (Join-Path $projectDir 'bin\AC4Tools.asi')
if ($LASTEXITCODE -ne 0) { throw 'Could not inspect ASI imports.' }
if (($imports -join "`n") -match '(?i)xinput[^\s]*\.dll') { throw 'ASI has a static XInput dependency.' }
$version = [Diagnostics.FileVersionInfo]::GetVersionInfo((Join-Path $projectDir 'bin\AC4Tools.asi'))
if ($version.FileVersion -ne '1.08' -or $version.FileBuildPart -ne 8) { throw 'ASI version metadata is inconsistent.' }
$fixtureRoot = Join-Path $projectDir 'obj\tests'
$fixture = Join-Path $fixtureRoot ("loader space's-" + [Guid]::NewGuid().ToString('N'))
$fixtureProject = Join-Path $fixture 'project'
$gameDir = Join-Path $fixture 'game'
New-Item -ItemType Directory -Path $gameDir -Force | Out-Null

function Expect-Failure {
    param([scriptblock] $Action, [string] $Pattern)
    try { & $Action | Out-Null }
    catch {
        if ($_.Exception.Message -notmatch $Pattern) { throw }
        return
    }
    throw "Expected failure matching: $Pattern"
}

try {
    # Exercise the actual scripts in an isolated project, never corrupting the
    # real bundle, release, installed loader, or running game for negative tests.
    foreach ($file in @('package_release.ps1', 'install_to_game.ps1', 'README.md', 'AC4Tools.ini',
                        'tools\loader-integrity.ps1', 'release_assets\loader-manifest.json',
                        'release_assets\dinput8.dll', 'release_assets\Ultimate-ASI-Loader-LICENSE.txt')) {
        $dest = Join-Path $fixtureProject $file
        New-Item -ItemType Directory -Path (Split-Path -Parent $dest) -Force | Out-Null
        Copy-Item -LiteralPath (Join-Path $projectDir $file) -Destination $dest
    }
    . (Join-Path $fixtureProject 'tools\loader-integrity.ps1')
    $package = Join-Path $fixtureProject 'package_release.ps1'
    $install = Join-Path $fixtureProject 'install_to_game.ps1'
    $manifest = Join-Path $fixtureProject 'release_assets\loader-manifest.json'
    $bundle = Join-Path $fixtureProject 'release_assets\dinput8.dll'
    $asi = Join-Path $projectDir 'bin\AC4Tools.asi'
    & $package -AsiPath $asi
    $releaseLoader = Join-Path $fixtureProject 'release\dinput8.dll'
    Assert-AC4ToolsLoader -Path $releaseLoader -ManifestPath $manifest

    $bundleSaved = $bundle + '.saved'
    Move-Item -LiteralPath $bundle -Destination $bundleSaved
    Expect-Failure { & $package -AsiPath $asi } 'loader not found'
    Move-Item -LiteralPath $bundleSaved -Destination $bundle
    [IO.File]::WriteAllText($bundle, 'corrupt-bundled-loader')
    Expect-Failure { & $package -AsiPath $asi } 'SHA256 mismatch'
    Copy-Item -LiteralPath (Join-Path $projectDir 'release_assets\dinput8.dll') -Destination $bundle -Force

    New-Item -ItemType Directory -Path (Join-Path $gameDir 'scripts') -Force | Out-Null
    $loader = Join-Path $gameDir 'dinput8.dll'
    $ini = Join-Path $gameDir 'scripts\AC4Tools.ini'
    [IO.File]::WriteAllText($ini, "[Hotkeys]`r`nMenuOpen=77`r`n[Game]`r`nTimeScale=0.25`r`n")
    & $install -GameDir $gameDir | Out-Null
    Assert-AC4ToolsLoader -Path $loader -ManifestPath $manifest
    $text = [IO.File]::ReadAllText($ini)
    if ($text -notmatch 'MenuOpen\s*=\s*77' -or $text -notmatch 'TimeScale\s*=\s*0.25') { throw 'INI settings were not preserved.' }
    if (-not (Test-Path -LiteralPath (Join-Path $gameDir 'scripts\AC4Tools.asi'))) { throw 'ASI was not installed.' }

    # A matching mapped/locked loader can be verified and left untouched.
    $mtime = [IO.File]::GetLastWriteTimeUtc($loader)
    $lockedLoader = [IO.File]::Open($loader, [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::Read)
    try { & $install -GameDir $gameDir | Out-Null }
    finally { $lockedLoader.Dispose() }
    if ([IO.File]::GetLastWriteTimeUtc($loader) -ne $mtime) { throw 'Verified loader was rewritten.' }

    [IO.File]::WriteAllText($releaseLoader, 'corrupt-release-loader')
    $beforeIni = [IO.File]::ReadAllText($ini)
    Expect-Failure { & $install -GameDir $gameDir } 'SHA256 mismatch'
    if ([IO.File]::ReadAllText($ini) -ne $beforeIni) { throw 'Rejected install changed the INI.' }
    Assert-AC4ToolsLoader -Path $loader -ManifestPath $manifest
    Copy-Item -LiteralPath $bundle -Destination $releaseLoader -Force

    [IO.File]::WriteAllText($loader, 'different-existing-loader')
    Expect-Failure { & $install -GameDir $gameDir } 'SHA256 mismatch'
    if ([IO.File]::ReadAllText($loader) -ne 'different-existing-loader') { throw 'Unknown installed loader was overwritten.' }
    if ([IO.File]::ReadAllText($ini) -ne $beforeIni) { throw 'Rejected install changed the INI.' }
    if (-not (Test-Path -LiteralPath (Join-Path $fixtureProject 'release\LOADER-PROVENANCE.json'))) { throw 'Loader provenance missing.' }
    $sums = [IO.File]::ReadAllText((Join-Path $fixtureProject 'release\SHA256SUMS.txt'))
    if ($sums -notmatch 'dinput8.dll') { throw 'Loader missing from package checksums.' }
    Write-Host 'PASS: verified loader bundled/installed; missing or corrupt loaders rejected; matching locked loader and INI preserved.'
} finally {
    $resolved = (Resolve-Path -LiteralPath $fixture).Path
    $allowed = (Resolve-Path -LiteralPath $fixtureRoot).Path + [IO.Path]::DirectorySeparatorChar
    if (-not $resolved.StartsWith($allowed, [StringComparison]::OrdinalIgnoreCase)) { throw 'Fixture escaped test root.' }
    Remove-Item -LiteralPath $resolved -Recurse -Force
}
