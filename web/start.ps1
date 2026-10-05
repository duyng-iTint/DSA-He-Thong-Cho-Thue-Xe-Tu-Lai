$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$build = Join-Path $root 'build/web-integration'

cmake -S $root -B $build
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
cmake --build $build --config Release --target RentalWebCore -j 4
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$candidates = @(
    (Join-Path $build 'RentalWebCore.exe'),
    (Join-Path $build 'Release/RentalWebCore.exe'),
    (Join-Path $build 'Debug/RentalWebCore.exe')
)
$core = $candidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $core) { throw 'Không tìm thấy executable RentalWebCore sau khi build.' }
$env:RENTAL_WEB_CORE = $core
node (Join-Path $PSScriptRoot 'server.js')
