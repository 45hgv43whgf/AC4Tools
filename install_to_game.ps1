param(
    [Parameter(Mandatory = $true)]
    [string] $GameDir
)

$ErrorActionPreference = "Stop"

$sourceDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$releaseDir = Join-Path $sourceDir "release"

if (-not (Test-Path -LiteralPath $releaseDir)) {
    & (Join-Path $sourceDir "package_release.bat")
    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}

if (-not (Test-Path -LiteralPath $GameDir)) {
    throw "GameDir does not exist: $GameDir"
}

Copy-Item -LiteralPath (Join-Path $releaseDir "dinput8.dll") -Destination (Join-Path $GameDir "dinput8.dll") -Force
New-Item -ItemType Directory -Force -Path (Join-Path $GameDir "scripts") | Out-Null
Copy-Item -LiteralPath (Join-Path $releaseDir "scripts\AC4Tools.asi") -Destination (Join-Path $GameDir "scripts\AC4Tools.asi") -Force
Copy-Item -LiteralPath (Join-Path $releaseDir "scripts\AC4Tools.ini") -Destination (Join-Path $GameDir "scripts\AC4Tools.ini") -Force

Get-Item -LiteralPath (Join-Path $GameDir "dinput8.dll"),
                      (Join-Path $GameDir "scripts\AC4Tools.asi"),
                      (Join-Path $GameDir "scripts\AC4Tools.ini") |
    Select-Object FullName, Length, LastWriteTime
