# Test Runner Scripts

Quick reference for running tests in Meowstro.

## Windows

### Option 1: Batch File (Recommended)
```cmd
test.bat
```

### Option 2: PowerShell
```powershell
.\test.ps1
```

## Linux/macOS

### Shell Script
```bash
./test.sh
```

## What the Scripts Do

1. **Build the project** (including tests)
2. **Run Google Test executable** with detailed output
3. **Show CTest summary** for CI compatibility
4. **Display colored status** (✅ pass / ❌ fail)
5. **Exit with proper codes** for CI integration

## Adding New Tests

1. Create new test files in appropriate directory.
2. Add the test file to `CMakeLists.txt` in the `meowstro_tests` target
3. Rebuild and run tests

The test runners will automatically pick up new tests!