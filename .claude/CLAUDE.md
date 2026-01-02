# OrderParserProcessor Project

## Project Overview
OrderParserProcessor is a C++ backend service that processes trading scripts using ANTLR-based parsing and gRPC communication.

## Architecture
- **Language**: C++17
- **Build System**: CMake (minimum version 3.28.3)
- **RPC Framework**: gRPC with Protocol Buffers
- **Parser**: ANTLR4 (version from `ANTLR_VERSION` env var)
- **Server**: gRPC server running on `0.0.0.0:50051`

## Project Structure
```
OrderParserProcessor/
├── backend/                 # Main C++ backend service
│   ├── src/
│   │   ├── services/       # gRPC service implementations
│   │   ├── processors/     # Business logic processors
│   │   ├── handlers/       # Request handlers
│   │   └── visitors/       # ANTLR visitor implementations
│   ├── includes/           # Header files
│   └── CMakeLists.txt
├── connectivity/            # Network connectivity module
├── proto/                   # Protocol buffer definitions
│   ├── messages/           # Message definitions
│   └── services/           # Service definitions
├── generated/cpp/          # Auto-generated code
│   ├── antlr/             # ANTLR generated parsers
│   ├── messages/          # Protobuf message classes
│   └── services/          # gRPC service stubs
└── rules/parser/          # ANTLR grammar files
    └── FiScript.g4        # FiScript grammar definition
```

## Key Components

### Libraries
- **lib_grpc_services**: gRPC service implementations
- **lib_grpc_messages**: Protocol buffer message classes
- **lib_services**: Business service layer
- **lib_processors**: Order processing logic
- **lib_handlers**: Request handling layer
- **FiScriptGrammarLib**: ANTLR-generated parser library

### Main Entry Point
- **File**: [backend/main.cc](backend/main.cc)
- **Function**: `RunServer()` starts gRPC server
- **Port**: 50051 (insecure credentials for development)

## Build Commands

### Standard Build
```bash
cmake -B build
cmake --build build
```

### Build with Tests
```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

### Clean Build
```bash
rm -rf build
cmake -B build
cmake --build build
```

## Environment Variables

**Required environment variables:**
```bash
export ORDER_PARSER_PROCESSOR_ROOT="/home/alexandre/Projects/OrderParserProcessor/OrderParserProcessor"
export ANTLR_VERSION="antlr-4.13.0"
```

**Useful ANTLR aliases:**
```bash
alias antlr4='java -Xmx500M -cp "/usr/local/lib/java/${ANTLR_VERSION}-complete.jar:$CLASSPATH" org.antlr.v4.Tool'
alias grun='java -Xmx500M -cp "/usr/local/lib/java/${ANTLR_VERSION}-complete.jar:$CLASSPATH" org.antlr.v4.gui.TestRig'
```

With these aliases:
- `antlr4` - Run ANTLR code generator directly
- `grun` - Test grammar and visualize AST from input text

## Dependencies
- **Protobuf**: Found via `find_package(Protobuf CONFIG REQUIRED)`
- **gRPC**: Found via `find_package(gRPC CONFIG REQUIRED)`
- **ANTLR4**: Custom external integration (ExternalAntlr4Cpp)
- **ANTLR Tool**: Located at `/usr/local/lib/java/antlr-4.13.0-complete.jar`

## Code Generation

### Protocol Buffers

**Important**: Proto files must be generated manually (CMake does NOT handle this automatically).

#### C++ Generation Commands
```bash
# Generate gRPC services
protoc -I$ORDER_PARSER_PROCESSOR_ROOT/proto \
  --cpp_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/cpp/messages/ \
  --grpc_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/cpp/ \
  --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
  $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/services/ -iname "*.proto")

# Generate protobuf messages
protoc -I$ORDER_PARSER_PROCESSOR_ROOT/proto/messages \
  --cpp_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/cpp/messages/ \
  $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/messages/ -iname "*.proto")
```

#### Python Generation Commands
```bash
# Generate gRPC services
python3 -m grpc_tools.protoc \
  --proto_path=$ORDER_PARSER_PROCESSOR_ROOT/proto \
  --python_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
  --pyi_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
  --grpc_python_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
  $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/services/ -iname "*.proto")

# Generate protobuf messages
python3 -m grpc_tools.protoc \
  --proto_path=$ORDER_PARSER_PROCESSOR_ROOT/proto \
  --python_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
  $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/messages/ -iname "*.proto")
```

Generated files:
- C++ Messages: `generated/cpp/messages/*.pb.cc`
- C++ Services: `generated/cpp/services/*.pb.cc`
- Python: `generated/python/*_pb2.py` and `*_pb2_grpc.py`

### ANTLR Parser

**CMake handles ANTLR `.g4` file generation automatically.**

Grammar defined in `rules/parser/FiScript.g4`
- Generated code: `generated/cpp/antlr/`
- Visitor pattern enabled
- CMake automatically regenerates when `.g4` files change

**For grammar development**: When making significant `.g4` modifications, work directly with `antlr4` command to avoid recompiling every time:
```bash
# Generate parser for testing
antlr4 -Dlanguage=Cpp -visitor rules/parser/FiScript.g4

# Visualize AST from input
grun FiScript <start_rule> -gui < input.txt
```

## Coding Conventions
- **C++ Standard**: C++17 (required)
- **Memory Management**: Prefer smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- **Includes**: Use project includes for local headers (e.g., `"handlers/script_submit_handler.h"`)
- **Namespaces**: Check existing code patterns before adding new namespaces
- **Comments**: Only add comments to explain non-obvious design choices or the reasoning behind decisions that aren't immediately clear from the code itself. Do NOT add comments that simply describe what the code does. Self-documenting code is preferred over excessive comments.
  - **Good example**: Explaining why `TryCancel()` is used instead of `Finish()` (see `reacton_service.cc:74-78`)
  - **Bad example**: Comments like "// Constructor" or "// Initialize the variable"

## Git Workflow
- **Main Branch**: `main`
- **Feature Branches**: Use descriptive names (e.g., `CONNECTIVITY_BASICS`, `DOCKER_DB_SETUP`)
- **Current Branch**: Check with `git status`

## Docker Setup
Project includes dockerized components:
- Python backend
- PostgreSQL database
- See docker-compose or Dockerfile for details

## Testing
- Tests are optional (controlled by `BUILD_TESTS` CMake option)
- Test directory: `backend/test/`
- Run with: `ctest --test-dir build`

## Important Notes
- Proto files must be generated before building C++ code
- ANTLR grammar changes require regenerating parser code
- gRPC server uses insecure credentials (development only)
- Generated code is stored in version control (check `generated/` directory)

## Common Issues
- **ANTLR not found**: Ensure `ANTLR_VERSION` environment variable is set
- **Protobuf errors**: Check that proto files are generated in `generated/cpp/`
- **Link errors**: Verify all libraries are linked in correct dependency order
