cmake_minimum_required(VERSION 3.14)
project(GymgymServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(ODB_COMPILER odb REQUIRED)
find_library(ODB_LIBRARY NAMES odb REQUIRED)
find_library(ODB_PGSQL_LIBRARY NAMES odb-pgsql REQUIRED)
find_library(ODB_BOOST_LIBRARY NAMES odb-boost REQUIRED)
find_library(PQ_LIBRARY NAMES pq REQUIRED)
find_package(nlohmann_json REQUIRED)
find_library(CRYPT_LIBRARY crypt REQUIRED)

set(ODB_GENERATED_DIR ${CMAKE_BINARY_DIR}/odb-generated)
file(MAKE_DIRECTORY ${ODB_GENERATED_DIR})

set(ODB_MODEL_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model/UserRecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model/MuscleRecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model/EquipmentRecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model/ExerciseRecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model/TrainingPlanRecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model/TrainingPlanExerciseRecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model/PlannedSetRecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model/WorkoutSessionRecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model/WorkoutSessionExerciseRecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model/PerformedSetRecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model/BodyMeasurementRecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model/PersonalRecordRecord.h
)

set(ODB_GENERATED_SOURCES)
set(ODB_GENERATED_HEADERS)
set(ODB_GENERATED_INLINES)

foreach(header ${ODB_MODEL_HEADERS})
    get_filename_component(model_name ${header} NAME_WE)

    list(APPEND ODB_GENERATED_SOURCES
        ${ODB_GENERATED_DIR}/${model_name}-odb.cxx
    )

    list(APPEND ODB_GENERATED_HEADERS
        ${ODB_GENERATED_DIR}/${model_name}-odb.hxx
    )

    list(APPEND ODB_GENERATED_INLINES
        ${ODB_GENERATED_DIR}/${model_name}-odb.ixx
    )
endforeach()

set(ODB_SCHEMA_FILE ${ODB_GENERATED_DIR}/schema.sql)

add_custom_command(
    OUTPUT
        ${ODB_GENERATED_SOURCES}
        ${ODB_GENERATED_HEADERS}
        ${ODB_GENERATED_INLINES}
        ${ODB_SCHEMA_FILE}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ODB_GENERATED_DIR}
    COMMAND ${ODB_COMPILER}
            --database pgsql
            --generate-query
            --generate-session
            --generate-schema
            --schema-format sql
            --schema-name gym
            --profile boost/date-time
            --std c++11
            --output-dir ${ODB_GENERATED_DIR}
            -I ${CMAKE_CURRENT_SOURCE_DIR}/include
            -I ${CMAKE_CURRENT_SOURCE_DIR}/include/model
            ${ODB_MODEL_HEADERS}
    DEPENDS ${ODB_MODEL_HEADERS}
    COMMENT "Generating ODB C++ files and schema.sql"
    VERBATIM
)

add_custom_target(odb_generated
    DEPENDS
        ${ODB_GENERATED_SOURCES}
        ${ODB_GENERATED_HEADERS}
        ${ODB_GENERATED_INLINES}
        ${ODB_SCHEMA_FILE}
)

file(GLOB REPOSITORY_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/repository/*Repository.cpp
)

add_executable(GymgymServer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/gym_state.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/db/Database.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/network/Server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/network/ClientHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handler/RequestRouter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handler/AuthHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handler/UserHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handler/TrainingPlanHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handler/ExerciseHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handler/WorkoutSessionHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handler/MeasurementHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handler/PersonalRecordHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handler/MuscleHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handler/EquipmentHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handler/StatusHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/service/UserService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/service/TrainingPlanService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/service/ExerciseService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/service/WorkoutSessionService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/service/MeasurementService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/service/PersonalRecordService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/service/MuscleService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/service/EquipmentService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util/BcryptHasher.cpp
    ${REPOSITORY_SOURCES}
    ${ODB_GENERATED_SOURCES}
)

add_dependencies(GymgymServer odb_generated)

target_include_directories(GymgymServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model
    ${CMAKE_CURRENT_SOURCE_DIR}/include/db
    ${CMAKE_CURRENT_SOURCE_DIR}/include/repository
    ${CMAKE_CURRENT_SOURCE_DIR}/include/server
    ${CMAKE_CURRENT_SOURCE_DIR}/include/server/network
    ${CMAKE_CURRENT_SOURCE_DIR}/include/server/handler
    ${CMAKE_CURRENT_SOURCE_DIR}/include/server/service
    ${CMAKE_CURRENT_SOURCE_DIR}/include/util
    ${ODB_GENERATED_DIR}
)

target_link_libraries(GymgymServer
    ${ODB_LIBRARY}
    ${ODB_PGSQL_LIBRARY}
    ${ODB_BOOST_LIBRARY}
    ${PQ_LIBRARY}
    ${CRYPT_LIBRARY}
    nlohmann_json::nlohmann_json
)

add_executable(PrintUserRecords
    ${CMAKE_CURRENT_SOURCE_DIR}/src/db/Database.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/print_user_records.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/gym_state.cpp
    ${REPOSITORY_SOURCES}
    ${ODB_GENERATED_SOURCES}
)

add_dependencies(PrintUserRecords odb_generated)

set_target_properties(PrintUserRecords PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_include_directories(PrintUserRecords PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model
    ${CMAKE_CURRENT_SOURCE_DIR}/include/db
    ${CMAKE_CURRENT_SOURCE_DIR}/include/repository
    ${ODB_GENERATED_DIR}
)

target_link_libraries(PrintUserRecords
    ${ODB_LIBRARY}
    ${ODB_PGSQL_LIBRARY}
    ${ODB_BOOST_LIBRARY}
    ${PQ_LIBRARY}
)
