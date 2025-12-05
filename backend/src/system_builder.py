import os
import shutil
from pathlib import Path
import argparse

"""
args:
script_name
script_code
script_includes
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

output_dir.mkdir(parents=True, exist_ok=True)
src_output_dir.mkdir(exist_ok=True)
include_output_dir.mkdir(exist_ok=True)
src_processors_output_dir.mkdir(exist_ok=True)
include_processors_output_dir.mkdir(exist_ok=True)
src_common_output_dir.mkdir(exist_ok=True)
include_common_output_dir.mkdir(exist_ok=True)

project_name = script_name

shutil.copy(project_root + "/backend/includes/processors/common/timers.h", include_common_output_dir.resolve())
shutil.copy(project_root + "/backend/src/processors/common/timers.cc", src_common_output_dir.resolve())

cmake_text = f'''
cmake_minimum_required(VERSION 3.10)
project({project_name})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB processors_file_list 
    "${{CMAKE_SOURCE_DIR}}/src/*.cc"
    "${{CMAKE_SOURCE_DIR}}/src/processors/common/*.cc")

add_library(lib_processors STATIC ${{processors_file_list}})
target_include_directories(lib_processors PUBLIC ${{CMAKE_SOURCE_DIR}}/include)

add_executable(${{PROJECT_NAME}} main.cc)
target_link_libraries(${{PROJECT_NAME}} PUBLIC lib_processors)
'''

cmake_path = output_dir / "CMakeLists.txt"
cmake_path.write_text(cmake_text)
