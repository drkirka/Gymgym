cmake_minimum_required(VERSION 3.14)
project(GymgymClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GymgymClient
    src/main.cpp
)

target_link_libraries(GymgymClient
    ftxui::component
    ftxui::dom
    ftxui::screen
)
