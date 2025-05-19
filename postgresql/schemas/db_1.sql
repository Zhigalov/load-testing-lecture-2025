DROP SCHEMA IF EXISTS hello_schema CASCADE;
CREATE SCHEMA IF NOT EXISTS hello_schema;
CREATE TABLE IF NOT EXISTS hello_schema.users (
    name TEXT PRIMARY KEY,
    count INTEGER DEFAULT(1)
);

DROP SCHEMA IF EXISTS clck_schema CASCADE;
CREATE SCHEMA IF NOT EXISTS clck_schema;
CREATE TABLE IF NOT EXISTS clck_schema.urls (
    hash    TEXT NOT NULL,
    version INTEGER DEFAULT(1),
    url     TEXT NOT NULL UNIQUE,
    PRIMARY KEY (hash, version)
);