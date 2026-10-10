#include "sader/WorkerRepository.h"

#include <stdexcept>
#include <utility>

WorkerRepository::WorkerRepository(const std::string& connection_string)
    : connection_(connection_string) {
    // pqxx::connection сам бросает исключение при ошибке подключения.
    // Явная проверка is_open() — на случай, если соединение
    // было открыто, но потом закрылось.
    if (!connection_.is_open()) {
        throw std::runtime_error("Cannot open PostgreSQL connection");
    }
}

std::vector<WorkerRecord> WorkerRepository::findAllEnabled() const {
    std::vector<WorkerRecord> result;

    // pqxx::work — транзакция. RAII: коммитится при выходе
    // из scope, либо откатывается при исключении.
    pqxx::work txn(connection_);

    const pqxx::result rows = txn.exec(
        "SELECT id, name, description, factory_key, schema_json, enabled "
        "FROM workers WHERE enabled = true ORDER BY name"
    );

    for (const auto& row : rows) {
        WorkerRecord rec;
        rec.id = row["id"].as<int>();
        rec.name = row["name"].as<std::string>();
        rec.description = row["description"].as<std::string>();
        rec.factory_key = row["factory_key"].as<std::string>();
        rec.schema_json = row["schema_json"].as<std::string>();
        rec.enabled = row["enabled"].as<bool>();
        result.push_back(std::move(rec));
    }

    return result;
}
