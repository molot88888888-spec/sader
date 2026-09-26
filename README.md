# SADER

**Self-describing, discoverable execution runtime** для AI-агентов.

## Что это

SADER — слой между AI-агентом и инструментами (Worker'ами):

- **DISCOVER** — найти подходящие capability по описанию
- **DESCRIBE** — получить точный контракт Worker'а
- **CALL** — выполнить операцию с валидацией аргументов

## Сборка

```bash
cmake -S . -B build
cmake --build build
./build/sader
