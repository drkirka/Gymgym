# Database Backups

## Creating a Backup

Run the backup script to create a timestamped SQL dump:

```bash
cd database
bash backup.sh
```

Backups are saved to `database/backups/backup_YYYYMMDD_HHMMSS.sql`

## Restoring from a Backup

To restore a previous backup, use the restore script:

```bash
cd database
bash restore.sh backups/backup_20260428_143022.sql
```

This recreates the `postgres` database first, then imports the SQL dump.
