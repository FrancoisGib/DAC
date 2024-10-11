sudo --login --user=postgres psql

sudo -u postgres pg_basebackup -h ip_du_master -D /var/lib/postgresql/17/main -U replication -v -P

sudo -u postgres psql -c "CREATE USER replication REPLICATION LOGIN CONNECTION LIMIT 1 ENCRYPTED PASSWORD 'password';"