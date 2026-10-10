#pragma once

#include "WorkerRecord.h"

#include <pqxx/pqxx>

#include <string>
#include <vector>

// WorkerRepository — доступ к таблице workers в PostgreSQL.
// Владеет соединением с БД. RAII: соединение открывается
// в конструкторе, закрывается в деструкторе (как ifstream).
class WorkerRepository {
public:
    explicit WorkerRepository(const std::string& connection_string);

    // Возвращает всех Worker'ов, у которых enabled = true,
    // отсортированных по имени.
    std::vector<WorkerRecord> findAllEnabled() const;

private:
    // mutable — потому что pqxx::connection нужно мутировать
    // при каждом запросе, а наши методы — const.
    mutable pqxx::connection connection_;
};
