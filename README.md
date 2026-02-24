# FiScript

A simple scripting language that transpiles to isolated C++ projects dedicated to trading.

## The Problem

Building a trading system typically requires C++ experts, tight coordination between quant and engineering teams, and diving into a large sensitive codebase every time a client needs a new behaviour. Miscommunication is frequent, changes are slow, and a bug in one strategy can threaten the rest of the system.

## The Solution

FiScript lets you express a trading strategy in a simple script. The system takes that script, transpiles it into an isolated C++ project, compiles it, and runs it. Each script lives in its own process — if one fails, the others are unaffected.

This means:
- A quant can write and deploy a strategy without involving a C++ engineer
- Strategies can run on a server as close to the market as possible, or distributed across machines
- Clients can activate and deactivate their strategies through a web interface at any time

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        React Frontend                        │
│         Submit scripts · Monitor alerts · Activate/Stop      │
└────────────────────────┬────────────────────────────────────┘
                         │ HTTP + WebSocket
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                       Python API  (FastAPI)                  │
│   Receive scripts · Transpile · Compile · Manage processes   │
│              WebSocket broadcast to frontend                 │
└──────┬──────────────────────────────────────┬───────────────┘
       │ gRPC                                 │ gRPC
       ▼                                      ▼
┌─────────────────┐                ┌──────────────────────────┐
│   C++ Backend   │                │  Running Script Processes │
│  Parse · Build  │                │  SendAlert · SendOrder    │
│  (ANTLR4 · gRPC)│                │  (generated C++ projects) │
└─────────────────┘                └──────────────────────────┘
       │ gRPC
       ▼
┌─────────────────┐
│   Distributor   │
│  Market data    │
│  price stream   │
└─────────────────┘
```

### Components

| Component | Tech | Role |
|---|---|---|
| Frontend | React + Vite | Submit scripts, view alerts, activate/deactivate |
| Python API | FastAPI + uvicorn | Orchestrates everything, manages processes, WebSocket |
| C++ Backend | gRPC server | Parses FiScript, generates and compiles C++ projects |
| Distributor | gRPC server | Streams market data (price updates) |
| Script processes | generated C++ | Isolated per-script binaries, report back via gRPC |

## FiScript Language

### Statements

```
// this will send an order every 10 seconds
// 50 times
Schedule(Start, 10s, 50) {
    SendOrder("AAPL", 100, 182.5)
}

// automatically subscribes to the market-data distributor
// listen to price updates for instrument "AAPL"
// if the price of the quote (price_update) is above
// 180, it first notifies the user
// then sends an Order on this instrument at this price
ReactOn("AAPL", 50) {
    if (quote.price > 180) {
        Alert("Price threshold reached")
        SendOrder("AAPL", 50, price)
    }
}
```

| Statement | Description |
|---|---|
| `Schedule(Start, x, iterations) { }` | Execute a block every x seconds, iterations times |
| `ReactOn(instrumentID, iterations) { }` | Execute a block when a market event fires, iterations times |
| `SendOrder(ticker, qty, price)` | Place an order |
| `Alert(message)` | Send a notification to the frontend |
| `Print(expression)` | Debug output, used by developpers |
| `if / else if / else` | Conditional logic |
| `x = expression` | Variable declaration |
| `x += expression` | Compound assignment (`+=` `-=` `*=` `/=`) |

Full grammar: [rules/parser/FiScript.g4](rules/parser/FiScript.g4)

## Quick Start

### Prerequisites

```bash
export ORDER_PARSER_PROCESSOR_ROOT="/path/to/OrderParserProcessor"
export ANTLR_VERSION="antlr-4.13.0"
```

### Docker (CI / isolated build)

## Frontend

npm run dev on the frontend

## Backend/Backend-API
```bash
sudo docker compose up
```
Note : dockerfiles and TMUX are not completly ready yet.
But you can find all the backend/backend-api dependencies
in the dockerfiles

## Project Structure

```
OrderParserProcessor/
├── backend/                # C++ gRPC server — parses FiScript, generates code
│   ├── src/
│   │   ├── services/       # gRPC service implementations
│   │   ├── processors/     # processing pipeline
│   │   ├── handlers/       # Request handlers
│   │   └── visitors/       # ANTLR visitor (transpiler)
│   └── includes/
├── backend-api/            # Python FastAPI — orchestration layer
│   ├── core/
│   │   ├── communication/  # gRPC clients/servers (communicator.py)
│   │   └── process_manager.py  # manages script processes
│   └── routes/views.py
├── connectivity/           # C++ Distributor — market data gRPC server
├── frontend/               # React + Vite frontend
├── proto/                  # Protocol Buffer definitions
│   ├── services/
│   └── messages/
├── generated/              # Auto-generated code (proto + ANTLR)
│   ├── cpp/
│   └── python/
├── rules/parser/
│   └── FiScript.g4         # FiScript grammar
├── docker/
│   ├── backend.Dockerfile
│   └── backend_api.Dockerfile
└── setuptmux.sh            # Start all services
```

## Development

### Regenerate Proto Files

After modifying `.proto` files:

```bash
# C++ services
protoc -I$ORDER_PARSER_PROCESSOR_ROOT/proto \
  --cpp_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/cpp/messages/ \
  --grpc_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/cpp/ \
  --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
  $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/services/ -iname "*.proto")

# C++ messages
protoc -I$ORDER_PARSER_PROCESSOR_ROOT/proto/messages \
  --cpp_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/cpp/messages/ \
  $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/messages/ -iname "*.proto")

# Python services
python3 -m grpc_tools.protoc \
  --proto_path=$ORDER_PARSER_PROCESSOR_ROOT/proto \
  --python_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
  --pyi_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
  --grpc_python_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
  $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/services/ -iname "*.proto")

# Python messages
python3 -m grpc_tools.protoc \
  --proto_path=$ORDER_PARSER_PROCESSOR_ROOT/proto \
  --python_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
  $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/messages/ -iname "*.proto")
```

### Modify the FiScript Grammar

Grammar is in [rules/parser/FiScript.g4](rules/parser/FiScript.g4). CMake regenerates the ANTLR parser automatically on build.

If you want to see the AST

```bash
antlr4 -Dlanguage=Cpp -visitor rules/parser/FiScript.g4
grun FiScript script -gui < input.txt
```

## License

See [LICENSE](LICENSE).
