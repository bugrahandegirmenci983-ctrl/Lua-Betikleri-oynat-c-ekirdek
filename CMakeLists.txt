cmake_minimum_required(VERSION 3.20)
project(myexecutor CXX)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(extern/luau)

add_executable(myexecutor main.cpp)

# SIRA ÖNEMLİ: Compiler önce, VM sonra
target_link_libraries(myexecutor PRIVATE 
    Luau.Compiler 
    Luau.VM
)

target_include_directories(myexecutor PRIVATE 
    extern/luau/VM/include
    extern/luau/Compiler/include
)