-- SADER database schema
-- PostgreSQL
--
-- Хранит метаданные Worker'ов: имя, описание, схему аргументов.
-- Код Worker'ов остаётся в C++; в БД — только метаданные.

CREATE TABLE IF NOT EXISTS workers (
    id SERIAL PRIMARY KEY,
    name TEXT UNIQUE NOT NULL,
    description TEXT NOT NULL,
    factory_key TEXT NOT NULL,
    schema_json JSONB NOT NULL,
    enabled BOOLEAN NOT NULL DEFAULT TRUE,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_workers_name ON workers(name);
CREATE INDEX IF NOT EXISTS idx_workers_enabled ON workers(enabled);
