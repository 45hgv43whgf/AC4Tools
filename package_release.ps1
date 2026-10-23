param([string] $AsiPath)
$ErrorActionPreference = 'Stop'
$projectDir = $PSScriptRoot
if (-not $AsiPath) { $AsiPath = Join-Path $projectDir 'bin\AC4Tools.asi' }
if (-not (Test-Path -LiteralPath $AsiPath -PathType Leaf)) { throw "ASI not found: $AsiPath" }
$releaseDir = Join-Path $projectDir 'release'
$scriptsDir = Join-Path $releaseDir 'scripts'
New-Item -ItemType Directory -Path $scriptsDir -Force | Out-Null
Copy-Item -LiteralPath $AsiPath -Destination (Join-Path $scriptsDir 'AC4Tools.asi') -Force
Copy-Item -LiteralPath (Join-Path $projectDir 'AC4Tools.ini') -Destination $scriptsDir -Force
Copy-Item -LiteralPath (Join-Path $projectDir 'README.md') -Destination (Join-Path $releaseDir 'README.txt') -Force

# Remove only the legacy generated loader; never recursively delete a directory.
# No third-party loader binary is trusted or distributed by this project.
$legacyLoader = Join-Path $releaseDir 'dinput8.dll'
if (Test-Path -LiteralPath $legacyLoader) { Remove-Item -LiteralPath $legacyLoader -Force }
$files = @('scripts\AC4Tools.asi', 'scripts\AC4Tools.ini', 'README.txt')
$checksums = foreach ($file in $files) {
    $stream = [IO.File]::OpenRead((Join-Path $releaseDir $file))
    $sha = [Security.Cryptography.SHA256]::Create()
    try { $hash = [BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-', '') }
    finally { $stream.Dispose(); $sha.Dispose() }
    '{0}  {1}' -f $hash, $file
}
$checksums | Set-Content -LiteralPath (Join-Path $releaseDir 'SHA256SUMS.txt') -Encoding Ascii
Write-Host "Prepared $releaseDir (install an ASI loader separately)."
