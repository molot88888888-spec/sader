-- SADER: заполнение таблицы workers
-- Применять: psql -h localhost -U sader -d sader -f db/seed.sql

-- Очищаем существующие записи, чтобы можно было перезапускать
TRUNCATE workers RESTART IDENTITY;

INSERT INTO workers (name, description, factory_key, schema_json) VALUES
('hash', 'Calculate md5 and sha256 hashes for text', 'hash',
 '{"arguments":[{"name":"operation","type":"string","required":true,"description":"md5 | sha256"},{"name":"text","type":"string","required":true,"description":"Input text"}]}'),

('file', 'File operations: read content, get size, check existence', 'file',
 '{"arguments":[{"name":"operation","type":"string","required":true,"description":"read | size | exists"},{"name":"path","type":"string","required":true,"description":"Path to file"}]}'),

('json', 'JSON operations: validate, get value by path, get type', 'json',
 '{"arguments":[{"name":"operation","type":"string","required":true,"description":"parse | get | type"},{"name":"json","type":"string","required":true,"description":"JSON content"},{"name":"path","type":"string","required":false,"description":"Dot-separated path"}]}'),

('http', 'HTTP GET requests: fetch body or status code from an http(s) URL', 'http',
 '{"arguments":[{"name":"operation","type":"string","required":true,"description":"get | status"},{"name":"url","type":"string","required":true,"description":"URL"}]}'),

('text', 'Text operations: count characters, words, or lines in a string', 'text',
 '{"arguments":[{"name":"operation","type":"string","required":true,"description":"length | words | lines"},{"name":"text","type":"string","required":true,"description":"Input text"}]}'),

('csv', 'CSV operations: count rows, sum/mean/min/max of a numeric column', 'csv',
 '{"arguments":[{"name":"operation","type":"string","required":true,"description":"count | sum | mean | min | max"},{"name":"data","type":"string","required":true,"description":"CSV content"},{"name":"column","type":"string","required":false,"description":"Column index"}]}'),

('process', 'Run allowed local commands and capture stdout', 'process',
 '{"arguments":[{"name":"operation","type":"string","required":true,"description":"run"},{"name":"command","type":"string","required":true,"description":"Command from whitelist"}]}'),

('model', 'Vector operations: normalize, dot product, cosine similarity', 'model',
 '{"arguments":[{"name":"operation","type":"string","required":true,"description":"normalize | dot | cosine"},{"name":"a","type":"string","required":true,"description":"First vector"},{"name":"b","type":"string","required":false,"description":"Second vector"}]}');
