CREATE TABLE IF NOT EXISTS users (
	id BIGSERIAL PRIMARY KEY,
	name TEXT NOT NULL,
	email TEXT NOT NULL UNIQUE
);

INSERT INTO users (name, email) VALUES
	('Alice Smith', 'alice@example.com'),
	('Bob Johnson', 'bob@example.com'),
	('Carol Davis', 'carol@example.com')
ON CONFLICT (email) DO NOTHING;
