# Docker Compose Service Manager

This module provides utilities for managing Docker Compose services.

## Starting Services

Run `docker-compose up` to start all services defined in the `docker-compose.yml` file.
Use `docker-compose up -d` to start services in detached mode.

## Stopping Services

Run `docker-compose down` to stop and remove all running containers defined in the compose file.
Use `docker-compose stop` to stop services without removing containers.

## Common Commands

- `docker-compose ps` - List running services
- `docker-compose logs` - View service logs
- `docker-compose restart` - Restart services
- `docker-compose build` - Build or rebuild service images