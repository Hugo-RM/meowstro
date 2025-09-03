@echo off
REM ===== test.bat =====
REM Simple test runner for Meowstro

echo Running Meowstro Tests...

REM Build the project first (including tests)
echo Building project...
cmake --build build --config Debug

if %errorlevel% neq 0 (
    echo Build failed! Cannot run tests.
    exit /b 1
)

echo.
echo Running tests with detailed output...

REM Find and run the test executable
set "TEST_EXECUTABLE="
if exist ".\build\bin\Debug\meowstro_tests.exe" (
    set "TEST_EXECUTABLE=.\build\bin\Debug\meowstro_tests.exe"
) else if exist ".\build\bin\Debug\meowstro_tests" (
    set "TEST_EXECUTABLE=.\build\bin\Debug\meowstro_tests"
) else if exist ".\build\bin\meowstro_tests.exe" (
    set "TEST_EXECUTABLE=.\build\bin\meowstro_tests.exe"
) else if exist ".\build\bin\meowstro_tests" (
    set "TEST_EXECUTABLE=.\build\bin\meowstro_tests"
) else (
    echo ❌ Test executable not found!
    echo Searched for:
    echo   - .\build\bin\Debug\meowstro_tests.exe
    echo   - .\build\bin\Debug\meowstro_tests
    echo   - .\build\bin\meowstro_tests.exe
    echo   - .\build\bin\meowstro_tests
    exit /b 1
)

echo Running: %TEST_EXECUTABLE%
%TEST_EXECUTABLE%

if %errorlevel% equ 0 (
    echo.
    echo ✅ All tests passed!
    
    REM Also show CTest summary  
    echo.
    echo CTest Summary:
    pushd build
    ctest --output-on-failure -C Debug
    if %errorlevel% neq 0 (
        echo ❌ CTest failed!
        popd
        exit /b 1
    )
    popd
) else (
    echo.
    echo ❌ Some tests failed!
    exit /b 1
)

echo.
echo Testing complete!