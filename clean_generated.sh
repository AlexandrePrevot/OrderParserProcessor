#!/bin/bash

set -e

GENERATED_DIR="./generated"

if [ ! -d "$GENERATED_DIR" ]; then
  echo "Directory $GENERATED_DIR does not exist."
  exit 1
fi

echo "Cleaning generated protobuf files in $GENERATED_DIR ..."

# Delete common protoc-generated files
find "$GENERATED_DIR" -name '*_pb2.pyi' -delete
find "$GENERATED_DIR" -name '*_pb2.py' -delete
find "$GENERATED_DIR" -name '*_grpc.py' -delete
find "$GENERATED_DIR" -name '*.pb.cc' -delete
find "$GENERATED_DIR" -name '*.pb.h' -delete
find "$GENERATED_DIR" -name '*.h' -delete
find "$GENERATED_DIR" -name '*.cpp' -delete
find "$GENERATED_DIR" -name '*.tokens' -delete
find "$GENERATED_DIR" -name '*.interp' -delete

echo "Cleanup complete."
