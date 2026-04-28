#!/bin/bash

set -e

BACKUP_FILE="$1"

if [ -z "$BACKUP_FILE" ]; then
    echo "Usage: $0 <backup-file>"
    exit 1
fi

if [ ! -f "$BACKUP_FILE" ]; then
    echo "Backup file not found: $BACKUP_FILE"
    exit 1
fi

# Recreate the database so the restore can succeed on a non-empty instance.
docker-compose exec -T db sh -c 'dropdb -U postgres --if-exists --maintenance-db=template1 postgres'
docker-compose exec -T db sh -c 'createdb -U postgres --maintenance-db=template1 postgres'

docker-compose exec -T db psql -U postgres postgres < "$BACKUP_FILE"

echo "Restore completed from: $BACKUP_FILE"