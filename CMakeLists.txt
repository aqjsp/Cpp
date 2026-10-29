cmake_minimum_required(VERSION 3.14)
project(CppCoreComponents CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

add_executable(cpp_suite
    src/main.cpp
)

# 开启严格警告以确保工业级代码质量
if (MSVC)
    target_compile_options(cpp_suite PRIVATE /W4)
else()
    target_compile_options(cpp_suite PRIVATE -Wall -Wextra -pedantic)
endif()
