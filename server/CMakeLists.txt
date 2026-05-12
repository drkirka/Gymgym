cmake_minimum_required(VERSION 3.14)
project(GymgymServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(ODB_COMPILER odb REQUIRED)
find_library(ODB_LIBRARY NAMES odb REQUIRED)
find_library(ODB_PGSQL_LIBRARY NAMES odb-pgsql REQUIRED)
find_library(PQ_LIBRARY NAMES pq REQUIRED)

set(ODB_GENERATED_DIR ${CMAKE_BINARY_DIR}/odb-generated)
set(ODB_USER_RECORD_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/model/UserRecord.h)
set(ODB_USER_RECORD_GENERATED_HEADER ${ODB_GENERATED_DIR}/UserRecord-odb.hxx)
set(ODB_USER_RECORD_GENERATED_SOURCE ${ODB_GENERATED_DIR}/UserRecord-odb.cxx)

add_custom_command(
    OUTPUT ${ODB_USER_RECORD_GENERATED_HEADER} ${ODB_USER_RECORD_GENERATED_SOURCE}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ODB_GENERATED_DIR}
    COMMAND ${ODB_COMPILER}
            --database pgsql
            --generate-query
            --generate-session
            --std c++11
            --output-dir ${ODB_GENERATED_DIR}
            -I ${CMAKE_CURRENT_SOURCE_DIR}/include
            -I ${CMAKE_CURRENT_SOURCE_DIR}/include/model
            ${ODB_USER_RECORD_HEADER}
    DEPENDS ${ODB_USER_RECORD_HEADER}
    VERBATIM
)

add_custom_target(odb_user_record_generated
    DEPENDS ${ODB_USER_RECORD_GENERATED_HEADER} ${ODB_USER_RECORD_GENERATED_SOURCE}
)

add_executable(GymgymServer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/request_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/db/Database.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/db/UserRepository.cpp
    ${ODB_USER_RECORD_GENERATED_SOURCE}
)

add_dependencies(GymgymServer odb_user_record_generated)

target_include_directories(GymgymServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model
    ${CMAKE_CURRENT_SOURCE_DIR}/include/db
    ${ODB_GENERATED_DIR}
)

target_link_libraries(GymgymServer
    ${ODB_LIBRARY}
    ${ODB_PGSQL_LIBRARY}
    ${PQ_LIBRARY}
)

add_executable(PrintUserRecords
    ${CMAKE_CURRENT_SOURCE_DIR}/src/db/Database.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/print_user_records.cpp
    ${ODB_USER_RECORD_GENERATED_SOURCE}
)

add_dependencies(PrintUserRecords odb_user_record_generated)

set_target_properties(PrintUserRecords PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_include_directories(PrintUserRecords PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model
    ${CMAKE_CURRENT_SOURCE_DIR}/include/db
    ${ODB_GENERATED_DIR}
)

target_link_libraries(PrintUserRecords
    ${ODB_LIBRARY}
    ${ODB_PGSQL_LIBRARY}
    ${PQ_LIBRARY}
)
