---
name: build
description: Build the C++ backend using CMake. Use when user asks to compile, build, rebuild, or mentions CMake, compilation errors, or linking issues.
allowed-tools: Bash, Read
---

# Build Project Skill

Build the OrderParserProcessor C++ backend using CMake.

## Prerequisites

Ensure required environment variables are set:
- `ORDER_PARSER_PROCESSOR_ROOT`: Project root directory
- `ANTLR_VERSION`: ANTLR version (e.g., antlr-4.13.0)

Check with:
```bash
echo $ORDER_PARSER_PROCESSOR_ROOT
echo $ANTLR_VERSION
```

## Instructions

1. Verify environment variables are set
2. Check if build directory exists, if not it will be created by CMake
3. Run CMake configuration: `cmake -B build`
4. Run CMake build: `cmake --build build`
5. Report any compilation or linking errors found
6. If successful, confirm the build completed and show executable location

## Build Options

### Standard Build
```bash
cmake -B build
cmake --build build
```

### Build with Tests
```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
```

### Clean Rebuild
```bash
rm -rf build
cmake -B build
cmake --build build
```

### Parallel Build (faster)
```bash
cmake --build build -j4
```

## ANTLR Handling

CMake automatically handles ANTLR file generation from `.g4` grammar files.

**Important**: For significant grammar modifications, it's better to work directly with `antlr4` command to avoid recompiling every time:

```bash
# Generate parser from grammar (for testing)
antlr4 -Dlanguage=Cpp -visitor rules/parser/FiScript.g4

# Test grammar with input
grun FiScript <start_rule> -gui < input.txt
```

## Common Issues

- **ANTLR not found**: Check `ANTLR_VERSION` environment variable is set
- **Protobuf errors**: Ensure proto files are generated in `generated/cpp/` (use proto-gen skill)
- **Link errors**: Check library dependency order in CMakeLists.txt
- **ORDER_PARSER_PROCESSOR_ROOT not set**: Export it in your shell profile

## Success Criteria

- CMake configuration completes without errors
- All targets compile successfully
- Executable `OrderParserProcessor` is created in `build/backend/` directory
