@echo off
REM ===== test.default.bat =====
REM Copy this file to test.bat and customize if needed

echo 🧪 Running Meowstro Tests...

REM Build the project first (including tests)
echo Building project...
cmake --build build --config Debug

if %errorlevel% neq 0 (
    echo ❌ Build failed! Cannot run tests.
    exit /b 1
)

echo.
echo 📊 Running tests with detailed output...

REM Run tests directly for best output formatting
.\build\bin\Debug\meowstro_tests.exe

if %errorlevel% equ 0 (
    echo.
    echo ✅ All tests passed!
    
    REM Also show CTest summary
    echo.
    echo 📋 CTest Summary:
    cd build
    ctest --output-on-failure -C Debug
    cd ..
) else (
    echo.
    echo ❌ Some tests failed!
    exit /b 1
)

echo.
echo 🎉 Testing complete!