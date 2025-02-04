#指定 cmake 最低版本
cmake_minimum_required(VERSION 3.20)

#指定项目名称
project(sol)
#指定C++标准
set(CMAKE_CXX_STANDARD 20)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("src")