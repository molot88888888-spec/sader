#pragma once

#include "RedisCache.h"
#include "WorkerRecord.h"

#include <pqxx/pqxx>

#include <string>
#include <vector>

// WorkerRepository — доступ к таблице workers в PostgreSQL.
// При наличии RedisCache — кэширует результаты findAllEnabled (cache-aside).
class WorkerRepository {
public:
    // Соединение с PostgreSQL + ссылка на кэш.
    // cache_enabled = false — кэш отключён (полезно для тестов).
    explicit WorkerRepository(
        const std::string& connection_string,
        RedisCache& cache,
        bool cache_enabled = true
    );

    // Возвращает всех Worker'ов, у которых enabled = true.
    // Сначала пробует кэш, при промахе — PostgreSQL, потом кладёт в кэш.
    std::vector<WorkerRecord> findAllEnabled() const;

private:
    mutable pqxx::connection connection_;
    RedisCache& cache_;         // не владеет — ссылка
    bool cache_enabled_;
};
