#!/bin/bash
# ===== test.default.sh =====
# Copy this file to test.sh and customize if needed

# Colors for better output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

echo -e "${CYAN}🧪 Running Meowstro Tests...${NC}"

# Build the project first (including tests)
echo -e "${YELLOW}Building project...${NC}"
cmake --build build --config Debug

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Build failed! Cannot run tests.${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}📊 Running tests with detailed output...${NC}"

# Run tests directly for best output formatting
./build/bin/Debug/meowstro

# Check if we're on Linux (executable won't have .exe extension)
if [ -f "./build/bin/Debug/meowstro_tests" ]; then
    ./build/bin/Debug/meowstro_tests
else
    ./build/bin/Debug/meowstro_tests.exe
fi

if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}✅ All tests passed!${NC}"
    
    # Also show CTest summary
    echo ""
    echo -e "${CYAN}📋 CTest Summary:${NC}"
    cd build
    ctest --output-on-failure
    cd ..
else
    echo ""
    echo -e "${RED}❌ Some tests failed!${NC}"
    exit 1
fi

echo ""
echo -e "${MAGENTA}🎉 Testing complete!${NC}"