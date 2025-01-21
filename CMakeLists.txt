cmake_minimum_required(VERSION 3.30)
project(win32_test)

set(CMAKE_CXX_STANDARD 20)

add_executable(win32_test main.cpp)

target_link_libraries(win32_test PRIVATE user32 userenv netapi32)