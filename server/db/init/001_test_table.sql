CREATE TABLE IF NOT EXISTS test_users (
    id SERIAL PRIMARY KEY,
    name TEXT NOT NULL,
    email TEXT NOT NULL UNIQUE,
    created_at TIMESTAMP NOT NULL DEFAULT NOW()
);

INSERT INTO test_users (name, email) VALUES
    ('Alice Smith', 'alice@example.com'),
    ('Bob Johnson', 'bob@example.com'),
    ('Carol Davis', 'carol@example.com')
ON CONFLICT (email) DO NOTHING;