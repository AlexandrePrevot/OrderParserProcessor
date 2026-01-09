# OrderParserProcessor

A high-performance C++ gRPC service that parses and executes trading scripts written in FiScript, a domain-specific language for scheduling and reacting to market events.

## What Does This Do?

OrderParserProcessor enables users to submit trading scripts that define automated trading behaviors. The service:

1. **Accepts FiScript Programs** - Receives trading scripts via gRPC API
2. **Parses & Validates** - Uses ANTLR4 to parse FiScript grammar into an Abstract Syntax Tree (AST)
3. **Executes Trading Logic** - Processes `Schedule` and `ReactOn` blocks to coordinate trading actions
4. **Manages Variables** - Supports variable declarations, assignments, and expressions
5. **Responds Asynchronously** - Returns execution status via gRPC

### FiScript Language

FiScript is a custom scripting language designed for algorithmic trading strategies. It supports:

- **Schedule Blocks** - Execute trading actions at specific times
- **ReactOn Blocks** - Respond to market data updates or events
- **Variables** - Store and manipulate numeric and string values
- **Expressions** - Perform calculations with `+`, `-`, `*`, `/` operators
- **Print Statements** - Debug output for development

#### Example FiScript Program

```fiscript
price = 100.0
quantity = 50

Schedule(09:30:00) {
    Print("Market opened")
}

ReactOn(PriceUpdate, AAPL) {
    price += 1.5
    Print("Price updated")
}
```

## Architecture

```
┌─────────────────┐
│  Client (gRPC)  │
└────────┬────────┘
         │ ScriptSubmitRequest
         ▼
┌─────────────────────────────┐
│   gRPC Server (port 50051)  │
│  ┌──────────────────────┐   │
│  │ ScriptSubmitHandler  │   │
│  └──────────┬───────────┘   │
│             ▼               │
│  ┌──────────────────────┐   │
│  │ ScriptSubmitProcessor│   │
│  └──────────┬───────────┘   │
│             ▼               │
│  ┌──────────────────────┐   │
│  │  ANTLR4 Parser       │   │
│  │  (FiScript Grammar)  │   │
│  └──────────┬───────────┘   │
│             ▼               │
│  ┌──────────────────────┐   │
│  │  Visitor Pattern     │   │
│  │  (AST Traversal)     │   │
│  └──────────────────────┘   │
└─────────────────────────────┘
```

### Technology Stack

- **C++17** - Core programming language
- **gRPC** - High-performance RPC framework
- **Protocol Buffers** - Service and message definitions
- **ANTLR4** - Parser generator for FiScript grammar
- **CMake** - Build system

## Quick Start

### Prerequisites

```bash
# Set required environment variables
export ORDER_PARSER_PROCESSOR_ROOT="/path/to/OrderParserProcessor"
export ANTLR_VERSION="antlr-4.13.0"
```

### Build & Run

```bash
# Build the project
cmake -B build
cmake --build build

# Run the server
./build/backend/order_parser_processor
```

The gRPC server will start on `0.0.0.0:50051`.

### Submit a Script

Use any gRPC client to submit a FiScript program:

```protobuf
service ApiToCore {
    rpc ScriptSubmit (ScriptSubmitRequest) returns (SynchronousReply);
}

message ScriptSubmitRequest {
    string content = 1;      // FiScript program text
    string title = 2;        // Script title
    string summary = 3;      // Optional description
    string user = 4;         // User identifier
    Timestamp creation_time = 5;
}
```

## Project Structure

```
OrderParserProcessor/
├── backend/                 # C++ gRPC service
│   ├── src/                # Implementation files
│   ├── includes/           # Header files
│   └── main.cc             # Server entry point
├── proto/                  # Protocol buffer definitions
│   ├── services/          # gRPC service definitions
│   └── messages/          # Message schemas
├── rules/parser/          # ANTLR grammar
│   └── FiScript.g4        # FiScript language definition
├── generated/             # Auto-generated code
│   ├── cpp/               # C++ protobuf & ANTLR output
│   └── python/            # Python protobuf output
└── connectivity/          # Network connectivity module
```

## Development

### Build with Tests

```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

### Modifying FiScript Grammar

The FiScript grammar is defined in [rules/parser/FiScript.g4](rules/parser/FiScript.g4). CMake automatically regenerates the parser when the grammar changes.

For interactive grammar development:

```bash
# Generate parser manually
antlr4 -Dlanguage=Cpp -visitor rules/parser/FiScript.g4

# Visualize AST from input
grun FiScript script -gui < test_input.txt
```

### Protocol Buffer Changes

After modifying `.proto` files, regenerate code:

```bash
# C++ generation
protoc -I$ORDER_PARSER_PROCESSOR_ROOT/proto \
  --cpp_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/cpp/messages/ \
  --grpc_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/cpp/ \
  --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
  $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/services/ -iname "*.proto")

# Python generation
python3 -m grpc_tools.protoc \
  --proto_path=$ORDER_PARSER_PROCESSOR_ROOT/proto \
  --python_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
  --grpc_python_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
  $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/services/ -iname "*.proto")
```

## FiScript Language Reference

### Statements

- `Schedule(args...) { ... }` - Execute block at scheduled time
- `ReactOn(args...) { ... }` - Execute block when event occurs
- `Print(expression)` - Output value for debugging
- `variable = expression` - Declare and initialize variable
- `variable += expression` - Compound assignment (`+=`, `-=`, `*=`, `/=`)

### Expressions

- Numeric literals: `42`, `3.14`
- String literals: `"AAPL"`, `"Hello"`
- Variables: `price`, `quantity`
- Arithmetic: `price * quantity`, `(total + 10) / 2`

### Grammar Rules

See [rules/parser/FiScript.g4](rules/parser/FiScript.g4) for complete grammar specification.

## Configuration

- **Server Port**: `50051` (configurable in [backend/main.cc](backend/main.cc#L8))
- **Credentials**: InsecureServerCredentials (development only)
- **C++ Standard**: C++17 (required)

## Contributing

When contributing code:

- Follow C++17 standards
- Use smart pointers for memory management
- Add comments only for non-obvious design decisions
- Run tests before submitting: `ctest --test-dir build`
- Update `.proto` files when changing API contracts

## License

See LICENSE file for details.

## More Information

For detailed development instructions, see [.claude/CLAUDE.md](.claude/CLAUDE.md).
