#pragma once

#include <optional>
#include <string>

// RedisCache — RAII-обёртка над hiredis.
// Владеет соединением с Redis, освобождает в деструкторе.
class RedisCache {
public:
    // Подключается к Redis по host:port.
    // Бросает std::runtime_error, если подключиться не удалось.
    explicit RedisCache(const std::string& host = "127.0.0.1", int port = 6379);

    // RAII: запрещаем копирование (одно соединение — один владелец).
    RedisCache(const RedisCache&) = delete;
    RedisCache& operator=(const RedisCache&) = delete;

    // Разрешаем перемещение — чтобы можно было хранить в unique_ptr.
    RedisCache(RedisCache&& other) noexcept;
    RedisCache& operator=(RedisCache&& other) noexcept;

    ~RedisCache();

    // Возвращает значение по ключу или std::nullopt, если ключа нет.
    std::optional<std::string> get(const std::string& key);

    // Устанавливает значение с TTL (в секундах).
    // Возвращает true при успехе.
    bool set(const std::string& key, const std::string& value, int ttl_seconds);

    // Удаляет ключ. Возвращает true, если ключ был.
    bool del(const std::string& key);

private:
    struct redisContext* context_ = nullptr;
};
