# it will be a mono image build because
# generated code will have to be compiled
# by this service
# so it needs gRPC to be able to compile them
FROM ubuntu:24.04@sha256:4fbb8e6a8395de5a7550b33509421a2bafbc0aab6c06ba2cef9ebffbc7092d90 AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    libssl-dev \
    zlib1g-dev \
    libabsl-dev \
    libc-ares-dev \
    libre2-dev \
    libboost-system-dev \
    nlohmann-json3-dev \
    openjdk-17-jre-headless \
    wget \
    && rm -rf /var/lib/apt/lists/*

ENV GRPC_RELEASE_TAG=v1.76.0

# --- building gRPC ---
# follow https://grpc.io/docs/languages/cpp/quickstart/#install-grpc
WORKDIR /opt
RUN git clone --recurse-submodules -b ${GRPC_RELEASE_TAG} --depth 1 --shallow-submodules https://github.com/grpc/grpc

WORKDIR /opt/grpc

RUN mkdir -p cmake/build && cd cmake/build && \
    cmake ../.. \
    -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DCMAKE_INSTALL_PREFIX=/opt/grpc_install && \
    make -j 3 && make install && \
    rm -rf /opt/grpc

# --- end building gRPC ---

ENV ANTLR_VERSION=antlr-4.13.0

# not making another image for ANTLR
# - very simple to get
# - the binary is still needed by current image
# to compile the backend
# --- ANTLR ---

WORKDIR /usr/local/lib/java

RUN wget -O /usr/local/lib/java/${ANTLR_VERSION}-complete.jar \
    https://www.antlr.org/download/${ANTLR_VERSION}-complete.jar && \
    echo "bc6f4abc0d225a27570126c51402569f000a8deda3487b70e7642840e570e4a6  /usr/local/lib/java/${ANTLR_VERSION}-complete.jar" | sha256sum -c -

# aliases to make it simpler to use antlr in the container (debug purposes) 
# WARNING: RUN is a distinc shell and alias is not persistant use script instead.
# --- end ANTLR ---

ENV ORDER_PARSER_PROCESSOR_ROOT=/app


# another stage in case we need to modify the docker file
# it avoids recompilation of gRPC
# but it doesn't save any space
FROM build AS app_build

# order is important here
# protos and rules should be copied
# first so that
# if modified, we recompile the entire backend
# because it will invalidate
# the layer from there
COPY proto /app/proto/
COPY rules /app/rules/
RUN mkdir -p /app/generated/cpp/messages/

RUN /opt/grpc_install/bin/protoc -I$ORDER_PARSER_PROCESSOR_ROOT/proto --cpp_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/cpp/messages/ --grpc_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/cpp/ --plugin=protoc-gen-grpc=/opt/grpc_install/bin/grpc_cpp_plugin $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/services/ -iname "*.proto")
RUN /opt/grpc_install/bin/protoc -I$ORDER_PARSER_PROCESSOR_ROOT/proto/messages --cpp_out=$ORDER_PARSER_PROCESSOR_ROOT/generated/cpp/messages/ $(find $ORDER_PARSER_PROCESSOR_ROOT/proto/messages/ -iname "*.proto")

COPY CMakeLists.txt /app/
COPY cmake /app/cmake/
COPY backend /app/backend/
COPY connectivity /app/connectivity/

WORKDIR /app/build

# -DCMAKE_PREFIX_PATH="/opt/grpc_install" necessary to find
# FindProtobuff.cmake
RUN cmake -DCMAKE_PREFIX_PATH="/opt/grpc_install" ../

# Change to RUN make -j$(nproc) in prod
RUN make -j 3

RUN mkdir -p /app/output_bin/

RUN useradd -m -s /bin/bash backend
# Need to know if there not others directory that backend will need
# to have accees.
RUN chown -R backend:backend /app/output_bin/
USER backend

EXPOSE 50051

ENTRYPOINT ["/app/build/backend/OrderParserProcessor"]
