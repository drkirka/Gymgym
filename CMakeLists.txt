cmake_minimum_required(VERSION 3.14)
project(Gymgym)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG v5.0.0
)

FetchContent_MakeAvailable(ftxui)

option(BUILD_CLIENT "Build client" ON)
option(BUILD_SERVER "Build server" ON)

if(BUILD_CLIENT)
    add_subdirectory(client)
endif()

if(BUILD_SERVER)
    add_subdirectory(server)
endif()
