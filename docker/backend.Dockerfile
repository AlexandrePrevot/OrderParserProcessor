# it will be a mono image build because
# generated code will have to be compiled
# by this service
# so it needs gRPC to be able to compile them
FROM ubuntu:24.04 AS build

# put everything that could be needed
# one time here
# didn't check if it was relevant
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    autoconf \
    libtool \
    pkg-config \
    unzip \
    libssl-dev \
    zlib1g-dev \
    libabsl-dev \
    libc-ares-dev \
    libre2-dev \
    nlohmann-json3-dev \
    libboost-all-dev \
    openjdk-17-jre-headless \
    wget

ENV GRPC_RELEASE_TAG=v1.76.0


# --- building gRPC ---
# follow https://grpc.io/docs/languages/cpp/quickstart/#install-grpc
WORKDIR /opt
RUN git clone --recurse-submodules -b ${GRPC_RELEASE_TAG} --depth 1 --shallow-submodules https://github.com/grpc/grpc

WORKDIR /opt/grpc

RUN mkdir -p cmake/build && cd cmake/build && \
    cmake ../.. -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF \
    -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DCMAKE_INSTALL_PREFIX=/opt/grpc_install && \
    make -j 3 && make install

# --- end building gRPC ---

ENV ANTLR_VERSION=antlr-4.13.0

# not making another image for ANTLR
# - very simple to get
# - the binary is still needed by current image
# to compile the backend
# --- ANTLR ---

WORKDIR /usr/local/lib/java

RUN wget -O /usr/local/lib/java/${ANTLR_VERSION}-complete.jar \
    https://www.antlr.org/download/${ANTLR_VERSION}-complete.jar

# aliases to make it simpler to use antlr in the container (debug purposes)
RUN alias antlr4='java -Xmx500M -cp "/usr/local/lib/java/${ANTLR_VERSION}-complete.jar:$CLASSPATH" org.antlr.v4.Tool'
RUN alias grun='java -Xmx500M -cp "/usr/local/lib/java/${ANTLR_VERSION}-complete.jar:$CLASSPATH" org.antlr.v4.gui.TestRig'
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
COPY generated /app/generated/

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
RUN make -j 3
