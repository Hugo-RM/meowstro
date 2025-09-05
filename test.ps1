# ===== test.ps1 =====
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

# Find and run the test executable
$testExecutablePaths = @(
    ".\build\bin\Debug\meowstro_tests.exe",
    ".\build\bin\Debug\meowstro_tests",
    ".\build\bin\meowstro_tests.exe",
    ".\build\bin\meowstro_tests"
)

$testExecutable = $null
foreach ($path in $testExecutablePaths) {
    if (Test-Path $path) {
        $testExecutable = $path
        break
    }
}

if (-not $testExecutable) {
    Write-Host "Test executable not found!" -ForegroundColor Red
    Write-Host "Searched for:" -ForegroundColor Red
    foreach ($path in $testExecutablePaths) {
        Write-Host "  - $path" -ForegroundColor Red
    }
    exit 1
}

Write-Host "Running: $testExecutable" -ForegroundColor Green
& $testExecutable
$testResult = $LASTEXITCODE

Write-Host ""
if ($testResult -eq 0) {
    Write-Host "All tests passed!" -ForegroundColor Green
    
    # Also show CTest summary
    Write-Host ""
    Write-Host "CTest Summary:" -ForegroundColor Cyan
    Push-Location build
    ctest --output-on-failure -C Debug
    $ctestResult = $LASTEXITCODE
    Pop-Location
    
    if ($ctestResult -ne 0) {
        Write-Host "CTest failed!" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "Some tests failed!" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Testing complete!" -ForegroundColor Magenta