# Gymgym

Gymgym is a client-server fitness management application written in C++. The project was developed as a study project to demonstrate client-server architecture, database integration, and modern C++ development practices.

The application allows users to manage training plans, workout sessions, exercises, body measurements, and personal records. Data is stored in PostgreSQL, while communication between the client and server is performed through TCP sockets.

## Features

- User management
- Training plan creation and management
- Exercise database
- Workout session tracking
- Body measurements
- Personal records
- PostgreSQL database integration
- Terminal-based user interface

## Technologies

The project is built using:

- C++17
- CMake
- PostgreSQL
- ODB ORM
- FTXUI
- nlohmann/json
- Docker

## Project Structure

Gymgym/
├── client/
│   ├── include/
│   ├── src/
│   └── CMakeLists.txt
│
├── server/
│   ├── include/
│   ├── src/
│   └── CMakeLists.txt
│
└── CMakeLists.txt
## Requirements

### General

- CMake 3.14+
- C++17 compatible compiler
- Git

### Server Dependencies

- PostgreSQL
- ODB Compiler
- libodb-dev
- libodb-pgsql-dev
- libodb-boost-dev
- libpq-dev

### Optional

- Docker
- Docker Compose

## Build

### Build the Server

cmake -S server -B build-server
cmake --build build-server --target GymgymServer -j
### Build the Client

cmake -S client -B build-client
cmake --build build-client -j
### Build the Entire Project

cmake -S . -B build
cmake --build build -j
## Running the Application

The application can be started using two terminal windows.

### Terminal 1

Start the database:

docker compose up db
or

```bash
docker start gymgym-db

Then start the server:

bash
./build-server/GymgymServer

The server listens on port `8080`.

### Terminal 2

Run the client:

bash
GYMGYM_HOST=127.0.0.1 GYMGYM_PORT=8080 ./build-client/gymgym_client

The client will connect to the server and provide an interactive terminal interface.

## Database

The server uses PostgreSQL as the primary storage system.

Default configuration:

text
Host: db
Database: postgres
Username: postgres
Password: pass
`

The database stores information about:

- Users
- Exercises
- Equipment
- Training plans
- Workout sessions
- Body measurements
- Personal records

## Educational Purpose

This project was created to practice:

- Object-oriented programming in C++
- Client-server application development
- Database integration
- ORM usage with ODB
- Socket programming
- Modern CMake project organization
- Terminal user interface development

## Authors

Developed as an educational software engineering project.
