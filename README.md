Gymgym
Gymgym is a client-server fitness management application developed in C++. The project was created as a study project to demonstrate the implementation of a distributed application using modern C++ technologies, a PostgreSQL database, and a terminal-based user interface.
The application allows users to manage workout plans, exercises, workout sessions, body measurements, and personal records. Communication between the client and the server is performed over TCP sockets, while all persistent data is stored in PostgreSQL.
Project Architecture
The project consists of two independent components:
Server
The server is responsible for:

processing client requests;
managing business logic;
communicating with the PostgreSQL database;
storing and retrieving application data;
handling user authentication and fitness-related records.

The server uses ODB ORM to map C++ classes to database tables.
Client
The client is a terminal application built with FTXUI. It provides an interactive interface for users and communicates with the server through a TCP connection.
The client allows users to:

create and manage accounts;
view and create training plans;
browse exercises;
create workout sessions;
record body measurements;
manage personal records.

Technologies
The project is built using the following technologies:

C++17
CMake
PostgreSQL
ODB ORM
FTXUI
nlohmann/json
Docker

Project Structure
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

Requirements
Before building the project, make sure the following tools are installed:
General

CMake 3.14 or newer
C++17 compatible compiler
Git

Server Dependencies

PostgreSQL
ODB Compiler
libodb-dev
libodb-pgsql-dev
libodb-boost-dev
libpq-dev

Optional

Docker
Docker Compose

Building the Project
Build the Server
cmake -S server -B build-server
cmake --build build-server --target GymgymServer -j

Build the Client
cmake -S client -B build-client
cmake --build build-client -j

Build Everything
cmake -S . -B build
cmake --build build -j

Running the Application
The application is typically started using two terminal windows.
Terminal 1 – Database and Server
Start the PostgreSQL database:
docker compose up db

or start an existing container:
docker start gymgym-db

After the database is running, start the server:
./build-server/GymgymServer

The server listens for incoming client connections on port 8080.
Terminal 2 – Client
Run the client application:
GYMGYM_HOST=127.0.0.1 GYMGYM_PORT=8080 ./build-client/gymgym_client

The client will connect to the server and provide an interactive terminal interface.
Database
The server uses PostgreSQL as the primary storage system.
Current default configuration:
Host: db
Database: postgres
Username: postgres
Password: pass

The database stores information about:

users;
exercises;
equipment;
training plans;
workout sessions;
body measurements;
personal records.

Educational Purpose
This project was developed to practice:

object-oriented design in C++;
client-server architecture;
database integration;
ORM usage with ODB;
socket programming;
modern CMake project organization;
terminal user interface development.

Authors
Gymgym development team.
