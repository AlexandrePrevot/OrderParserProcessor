import os
import shutil
from pathlib import Path
import argparse

"""
args:
scriptName
includes
"""

parser = argparse.ArgumentParser()

parser.add_argument(
    "--scriptName",
    nargs="*",
    required=True
)

parser.add_argument(
    "--includes",
    nargs="*",
    required=True
)

args = parser.parse_args()

script_name = args.scriptName[0]
includes = args.includes

print(f"building project for {script_name}")
print(f"includes are {includes}")

project_root = os.getenv("ORDER_PARSER_PROCESSOR_ROOT")
output_root = project_root + "/../output_bin"

shutil.rmtree(output_root)

output_dir = Path(output_root)
src_output_dir = output_dir / "src"
include_output_dir = output_dir / "include"

src_processors_output_dir = src_output_dir / "processors"
include_processors_output_dir = include_output_dir / "processors"

src_common_output_dir = src_processors_output_dir / "common"
include_common_output_dir = include_processors_output_dir / "common"

src_services_output_dir = src_output_dir / "services"
include_services_output_dir = include_output_dir / "services"

generated_output_dir = output_dir / "generated"
cpp_generated_output_dir = generated_output_dir / "cpp"
services_generated_output_dir = cpp_generated_output_dir / "services"
messages_generated_output_dir = cpp_generated_output_dir / "messages"

output_dir.mkdir(parents=True, exist_ok=True)
src_output_dir.mkdir(exist_ok=True)
include_output_dir.mkdir(exist_ok=True)
src_processors_output_dir.mkdir(exist_ok=True)
include_processors_output_dir.mkdir(exist_ok=True)
src_common_output_dir.mkdir(exist_ok=True)
include_common_output_dir.mkdir(exist_ok=True)
src_services_output_dir.mkdir(exist_ok=True)
include_services_output_dir.mkdir(exist_ok=True)
generated_output_dir.mkdir(exist_ok=True)
cpp_generated_output_dir.mkdir(exist_ok=True)
messages_generated_output_dir.mkdir(exist_ok=True)
services_generated_output_dir.mkdir(exist_ok=True)


project_name = script_name

for include in includes:
    include = include.removesuffix(".h")
    include = include.strip('"')

    print(f"copying include {include}")

    # Check if it's a service or processor include
    if include.startswith("services/"):
        # Service file
        include_file = include.removeprefix("services/")
        shutil.copy(project_root + "/backend/includes/services/" + include_file + ".h", include_services_output_dir.resolve())
        shutil.copy(project_root + "/backend/src/services/" + include_file + ".cc", src_services_output_dir.resolve())
    elif include.startswith("processors/common/"):
        # Processor file
        include_file = include.removeprefix("processors/common/")
        shutil.copy(project_root + "/backend/includes/processors/common/" + include_file + ".h", include_common_output_dir.resolve())
        shutil.copy(project_root + "/backend/src/processors/common/" + include_file + ".cc", src_common_output_dir.resolve())
    else:
        # Legacy support - assume it's in processors/common
        shutil.copy(project_root + "/backend/includes/" + include + ".h", include_common_output_dir.resolve())
        shutil.copy(project_root + "/backend/src/" + include + ".cc", src_common_output_dir.resolve())
shutil.copytree(project_root + "/generated/cpp/messages", messages_generated_output_dir.resolve(), dirs_exist_ok=True)
shutil.copytree(project_root + "/generated/cpp/services", services_generated_output_dir.resolve(), dirs_exist_ok=True)

cmake_text = f'''
cmake_minimum_required(VERSION 3.10)
project({project_name})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

file(GLOB grpc_services_list "${{CMAKE_SOURCE_DIR}}/generated/cpp/services/*.pb.cc")
add_library(lib_grpc_services STATIC ${{grpc_services_list}})
target_include_directories(lib_grpc_services PUBLIC ${{CMAKE_SOURCE_DIR}}/generated/cpp/services/)
target_include_directories(lib_grpc_services PUBLIC ${{CMAKE_SOURCE_DIR}}/generated/cpp/messages/)
target_include_directories(lib_grpc_services PUBLIC ${{CMAKE_SOURCE_DIR}}/generated/cpp/)
target_link_libraries(lib_grpc_services PUBLIC gRPC::grpc++ protobuf::libprotobuf)

file(GLOB grpc_messages_list "${{CMAKE_SOURCE_DIR}}/generated/cpp/messages/*.pb.cc")
add_library(lib_grpc_messages STATIC ${{grpc_messages_list}})
target_include_directories(lib_grpc_messages PUBLIC ${{CMAKE_SOURCE_DIR}}/generated/cpp/)
target_link_libraries(lib_grpc_messages PUBLIC gRPC::grpc++ protobuf::libprotobuf)

file(GLOB processors_file_list
    "${{CMAKE_SOURCE_DIR}}/src/*.cc"
    "${{CMAKE_SOURCE_DIR}}/src/processors/common/*.cc")

file(GLOB services_file_list
    "${{CMAKE_SOURCE_DIR}}/src/services/*.cc")


add_executable(${{PROJECT_NAME}} main.cc)

if(processors_file_list)
    add_library(lib_processors OBJECT ${{processors_file_list}})
    target_include_directories(lib_processors PUBLIC ${{CMAKE_SOURCE_DIR}}/include)
    target_link_libraries(${{PROJECT_NAME}} PUBLIC lib_processors)
else()
    message("lib_processors is empty, so no need to build this library")
endif()

if(services_file_list)
    add_library(lib_services OBJECT ${{services_file_list}})
    target_include_directories(lib_services PUBLIC ${{CMAKE_SOURCE_DIR}}/include)
    target_link_libraries(lib_services PUBLIC lib_grpc_services)
    target_link_libraries(lib_services PUBLIC lib_grpc_messages)
    target_link_libraries(lib_services PUBLIC gRPC::grpc++ protobuf::libprotobuf)
    target_link_libraries(${{PROJECT_NAME}} PUBLIC lib_services)
else()
    message("lib_services is empty, so no need to build this library")
endif()

target_link_libraries(${{PROJECT_NAME}} PUBLIC lib_grpc_services)
target_link_libraries(${{PROJECT_NAME}} PUBLIC lib_grpc_messages)
target_link_libraries(${{PROJECT_NAME}} PUBLIC gRPC::grpc++ protobuf::libprotobuf)
'''

cmake_path = output_dir / "CMakeLists.txt"
cmake_path.write_text(cmake_text)
