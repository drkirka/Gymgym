cmake_minimum_required(VERSION 3.14)
project(GymgymClient)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)
FetchContent_Declare(ftxui GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git GIT_TAG v5.0.0)
FetchContent_MakeAvailable(ftxui)
add_executable(gymgym_client
    src/main.cpp
    src/NetworkClient.cpp
    src/ClientApi.cpp
)
target_include_directories(gymgym_client PRIVATE include)

target_link_libraries(gymgym_client
    PRIVATE ftxui::screen ftxui::dom ftxui::component
)

if(WIN32)
    target_link_libraries(gymgym_client PRIVATE ws2_32)
endif()