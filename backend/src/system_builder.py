import os
from pathlib import Path

output_dir = Path("../../../output_bin")
output_dir.mkdir(parents=True, exist_ok=True)

project_name="TradingSystem"

cmake_text = f"""
cmake_minimum_required(VERSION 3.10)
project({project_name})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable({project_name}
    main.cc
)

target_include_directories({project_name} PRIVATE include)
"""

cmake_path = output_dir / "CMakeLists.txt"
cmake_path.write_text(cmake_text)