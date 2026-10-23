function Get-AC4ToolsSha256 {
    param([Parameter(Mandatory = $true)][string] $Path)
    $stream = [IO.File]::OpenRead($Path)
    $sha = [Security.Cryptography.SHA256]::Create()
    try { return [BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-', '') }
    finally { $stream.Dispose(); $sha.Dispose() }
}

function Assert-AC4ToolsLoader {
    param(
        [Parameter(Mandatory = $true)][string] $Path,
        [Parameter(Mandatory = $true)][string] $ManifestPath
    )
    $pin = Get-Content -LiteralPath $ManifestPath -Raw | ConvertFrom-Json
    if ($pin.dll_sha256 -notmatch '^[0-9A-Fa-f]{64}$') { throw "Invalid loader hash pin in $ManifestPath" }
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Pinned ASI loader not found: $Path" }
    $actual = Get-AC4ToolsSha256 -Path $Path
    if ($actual -ne $pin.dll_sha256) {
        throw "ASI loader SHA256 mismatch at $Path. Expected $($pin.dll_sha256); found $actual. Source: $($pin.release_url)"
    }
}
