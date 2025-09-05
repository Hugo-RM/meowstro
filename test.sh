#!/bin/bash
# ===== test.sh =====
# Simple test runner for Meowstro

# Colors for better output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

echo -e "${CYAN}Running Meowstro Tests...${NC}"

# Build the project first (including tests)
echo -e "${YELLOW}Building project...${NC}"
# Use --config Debug to ensure consistent build type across platforms
cmake --build build --config Debug

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Build failed! Cannot run tests.${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}Running tests with detailed output...${NC}"

# Find and run the test executable
if [ -f "./build/bin/Debug/meowstro_tests" ]; then
    TEST_EXECUTABLE="./build/bin/Debug/meowstro_tests"
elif [ -f "./build/bin/Debug/meowstro_tests.exe" ]; then
    TEST_EXECUTABLE="./build/bin/Debug/meowstro_tests.exe"
elif [ -f "./build/bin/meowstro_tests" ]; then
    TEST_EXECUTABLE="./build/bin/meowstro_tests"
else
    echo -e "${RED}❌ Test executable not found!${NC}"
    echo "Searched for:"
    echo "  - ./build/bin/Debug/meowstro_tests"
    echo "  - ./build/bin/Debug/meowstro_tests.exe"
    echo "  - ./build/bin/meowstro_tests"
    exit 1
fi

echo "Running: $TEST_EXECUTABLE"
$TEST_EXECUTABLE

if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}✅ All tests passed!${NC}"
    
    # Also show CTest summary
    echo ""
    echo -e "${CYAN}CTest Summary:${NC}"
    cd build
    # Try with -C Debug first (Windows), fall back to without config (Linux/macOS)
    ctest --output-on-failure -C Debug 2>/dev/null || ctest --output-on-failure
    CTEST_RESULT=$?
    cd ..
    
    if [ $CTEST_RESULT -ne 0 ]; then
        echo -e "${RED}❌ CTest failed!${NC}"
        exit 1
    fi
else
    echo ""
    echo -e "${RED}❌ Some tests failed!${NC}"
    exit 1
fi

echo ""
echo -e "${MAGENTA}Testing complete!${NC}"