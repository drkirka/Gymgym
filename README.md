# Gymgym

Split client/server C++ project. Server uses ODB + PostgreSQL; client uses FTXUI.

Quick start

Prerequisites (native build):
- `cmake`, `g++`, `build-essential`
- `odb`, `libodb-dev`, `libodb-pgsql-dev`, `libpq-dev` (for server ODB generation)

Build server (standalone):
```
cmake -S server -B build-server
cmake --build build-server --target GymgymServer -j
```

Build client (standalone):
```
cmake -S client -B build-client
cmake --build build-client --target GymgymClient -j
```

Top-level build (both subprojects):
```
cmake -S . -B build
cmake --build build -j
```

Docker:
```
docker compose up --build
```
This starts `db` (Postgres), `adminer`, `server` (built with ODB available in builder), and `client`.


