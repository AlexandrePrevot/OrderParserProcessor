FROM python:3.12 AS backend_api_deps_builder

ENV ORDER_PARSER_PROCESSOR_ROOT=/app
WORKDIR /app/backend-api

COPY backend-api/requirements.txt /app/backend-api 
RUN pip install --no-cache-dir -r requirements.txt

# ---- Generate gRPC ----
FROM backend_api_deps_builder AS backend_api_grpc_generator

WORKDIR /app

RUN mkdir -p /app/generated/python/
COPY proto /app/proto

RUN python3 -m grpc_tools.protoc \
    --proto_path=$ORDER_PARSER_PROCESSOR_ROOT/proto \
    --python_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
    --pyi_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
    --grpc_python_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
    $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/services/ -iname "*.proto")

RUN python3 -m grpc_tools.protoc \
    --proto_path=$ORDER_PARSER_PROCESSOR_ROOT/proto \
    --python_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/python/ \
    $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/messages/ -iname "*.proto")

# ---- Final container ----
FROM backend_api_grpc_generator AS backend_api_builder

COPY backend-api/ /app/backend-api
WORKDIR /app/backend-api

EXPOSE 8000

CMD ["fastapi", "dev", "main.py", "--host", "0.0.0.0"]
