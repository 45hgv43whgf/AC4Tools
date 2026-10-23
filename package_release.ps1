param([string] $AsiPath)
$ErrorActionPreference = 'Stop'
$projectDir = $PSScriptRoot
. (Join-Path $projectDir 'tools\loader-integrity.ps1')
if (-not $AsiPath) { $AsiPath = Join-Path $projectDir 'bin\AC4Tools.asi' }
if (-not (Test-Path -LiteralPath $AsiPath -PathType Leaf)) { throw "ASI not found: $AsiPath" }
$manifest = Join-Path $projectDir 'release_assets\loader-manifest.json'
$loader = Join-Path $projectDir 'release_assets\dinput8.dll'
# Validate against the upstream-derived pin before changing any release files.
Assert-AC4ToolsLoader -Path $loader -ManifestPath $manifest
$releaseDir = Join-Path $projectDir 'release'
$scriptsDir = Join-Path $releaseDir 'scripts'
New-Item -ItemType Directory -Path $scriptsDir -Force | Out-Null
Copy-Item -LiteralPath $AsiPath -Destination (Join-Path $scriptsDir 'AC4Tools.asi') -Force
Copy-Item -LiteralPath (Join-Path $projectDir 'AC4Tools.ini') -Destination $scriptsDir -Force
Copy-Item -LiteralPath (Join-Path $projectDir 'README.md') -Destination (Join-Path $releaseDir 'README.txt') -Force

Copy-Item -LiteralPath $loader -Destination (Join-Path $releaseDir 'dinput8.dll') -Force
Assert-AC4ToolsLoader -Path (Join-Path $releaseDir 'dinput8.dll') -ManifestPath $manifest
Copy-Item -LiteralPath $manifest -Destination (Join-Path $releaseDir 'LOADER-PROVENANCE.json') -Force
Copy-Item -LiteralPath (Join-Path $projectDir 'release_assets\Ultimate-ASI-Loader-LICENSE.txt') -Destination $releaseDir -Force
$files = @('scripts\AC4Tools.asi', 'scripts\AC4Tools.ini', 'README.txt', 'dinput8.dll', 'LOADER-PROVENANCE.json', 'Ultimate-ASI-Loader-LICENSE.txt')
$checksums = foreach ($file in $files) {
    $hash = Get-AC4ToolsSha256 -Path (Join-Path $releaseDir $file)
    '{0}  {1}' -f $hash, $file
}
$checksums | Set-Content -LiteralPath (Join-Path $releaseDir 'SHA256SUMS.txt') -Encoding Ascii
Write-Host "Prepared $releaseDir with the SHA256-verified Win32 ASI loader."
