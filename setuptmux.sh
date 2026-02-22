#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV="$REPO_ROOT/../venv"
BACKEND_BIN="$REPO_ROOT/../build/backend/OrderParserProcessor"
DISTRIBUTOR_BIN="$REPO_ROOT/../build/connectivity/Distributor"
BACKEND_API_DIR="$REPO_ROOT/backend-api"
FRONTEND_DIR="$REPO_ROOT/frontend/OrderParserProcessor"
SESSION="FiScript"

export ORDER_PARSER_PROCESSOR_ROOT="$REPO_ROOT"

if ! command -v tmux &>/dev/null; then
    echo "tmux is not installed. Run: sudo apt install tmux"
    exit 1
fi

if tmux has-session -t "$SESSION" 2>/dev/null; then
    echo "Session '$SESSION' is already running — attaching."
    tmux attach -t "$SESSION"
    exit 0
fi

if [[ ! -x "$BACKEND_BIN" ]]; then
    echo "Backend binary not found at $BACKEND_BIN"
    echo "Run: cmake -B build && cmake --build build"
    exit 1
fi

if [[ ! -x "$DISTRIBUTOR_BIN" ]]; then
    echo "Distributor binary not found at $DISTRIBUTOR_BIN"
    echo "Run: cmake -B build && cmake --build build"
    exit 1
fi

tmux new-session  -d -s "$SESSION" -n "backend"     \
    "bash -c 'trap : INT; cd $REPO_ROOT/backend && export ORDER_PARSER_PROCESSOR_ROOT=$REPO_ROOT && $BACKEND_BIN; exec bash -i'"

tmux new-window   -t "$SESSION"    -n "distributor" \
    "bash -c 'trap : INT; cd $REPO_ROOT/connectivity && $DISTRIBUTOR_BIN; exec bash -i'"

tmux new-window   -t "$SESSION"    -n "api"         \
    "bash -c 'trap : INT; source $VENV/bin/activate && cd $BACKEND_API_DIR && ORDER_PARSER_PROCESSOR_ROOT=$REPO_ROOT uvicorn main:app --host 0.0.0.0 --port 8000 --reload; exec bash -i'"

tmux new-window   -t "$SESSION"    -n "frontend"    \
    "bash -c 'trap : INT; cd $FRONTEND_DIR && npm run dev; exec bash -i'"

tmux select-window -t "$SESSION:backend"

echo "All services started in tmux session '$SESSION'."
echo "  Switch windows : Ctrl-b  then 0-3  (or n/p for next/prev)"
echo "  Detach         : Ctrl-b d"
echo "  Stop everything: ./stop.sh"

tmux attach -t "$SESSION"
