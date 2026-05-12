#!/bin/bash

# Database backup script
# Creates a timestamped SQL dump of the PostgreSQL database

BACKUP_DIR="./backups"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
BACKUP_FILE="$BACKUP_DIR/backup_$TIMESTAMP.sql"

# Create backups directory if it doesn't exist
mkdir -p "$BACKUP_DIR"

# Run pg_dump inside the container without a TTY so the dump stays clean SQL
docker-compose exec -T db pg_dump -U postgres postgres > "$BACKUP_FILE"

if [ $? -eq 0 ]; then
    echo "✓ Backup created: $BACKUP_FILE"
    ls -lh "$BACKUP_FILE"
else
    echo "✗ Backup failed"
    exit 1
fi
