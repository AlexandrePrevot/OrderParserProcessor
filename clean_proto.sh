#!/bin/bash

# Safety: exit immediately on error
set -e

# The directory to clean
GENERATED_DIR="./generated"

# Check if directory exists
if [ ! -d "$GENERATED_DIR" ]; then
  echo "Directory $GENERATED_DIR does not exist."
  exit 1
fi

echo "Cleaning generated protobuf files in $GENERATED_DIR ..."

# Delete common protoc-generated files
#find "$GENERATED_DIR" -name '*_pb2.py' -delete
find "$GENERATED_DIR" -name '*.pb.cc' -delete
find "$GENERATED_DIR" -name '*.pb.h' -delete

echo "Cleanup complete."
