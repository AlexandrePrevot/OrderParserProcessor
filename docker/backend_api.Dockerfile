# ---- image to generate gRPC ----
FROM python:3.12 AS backend_api_deps_builder

ENV ORDER_PARSER_PROCESSOR_ROOT=/app
WORKDIR /app/backend-api

COPY backend-api/requirements.txt /app/backend-api 
RUN pip install --no-cache-dir -r requirements.txt

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
FROM python:3.12-slim AS runtime

ENV ORDER_PARSER_PROCESSOR_ROOT=/app
WORKDIR /app/backend-api

# Copy requirements + gRPC generated files
# proto are needed to generate gRPC fiels so we don't mind
# not copying them
COPY --from=deps /usr/local /usr/local
COPY --from=deps /app/generated /app/generated
COPY backend-api/ /app/backend-api


EXPOSE 8000

CMD ["fastapi", "dev", "main.py", "--host", "0.0.0.0"]
