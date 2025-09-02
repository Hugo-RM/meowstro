# ===== test.default.ps1 =====
# Simple test runner for Meowstro

Write-Host "Running Meowstro Tests..." -ForegroundColor Cyan

# Build the project first (including tests)
Write-Host "Building project..." -ForegroundColor Yellow
cmake --build build --config Debug

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed! Cannot run tests." -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Running tests with detailed output..." -ForegroundColor Green

# Run tests directly for best output formatting
& ".\build\bin\Debug\meowstro_tests.exe"
$testResult = $LASTEXITCODE

Write-Host ""
if ($testResult -eq 0) {
    Write-Host "All tests passed!" -ForegroundColor Green
} else {
    Write-Host "Some tests failed!" -ForegroundColor Red
}

# Also show CTest summary
Write-Host ""
Write-Host "CTest Summary:" -ForegroundColor Cyan
Push-Location build
ctest --output-on-failure -C Debug
Pop-Location

Write-Host ""
Write-Host "Testing complete!" -ForegroundColor Magenta

exit $testResult