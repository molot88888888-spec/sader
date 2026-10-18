#include "sader/WorkerRepository.h"

#include <nlohmann/json.hpp>

#include <stdexcept>
#include <utility>

using json = nlohmann::json;

namespace {

constexpr const char* kCacheKey = "sader:workers:all";
constexpr int kCacheTtlSeconds = 60;

// Сериализует вектор WorkerRecord в JSON-строку.
std::string serializeRecords(const std::vector<WorkerRecord>& records) {
    json arr = json::array();
    for (const auto& r : records) {
        arr.push_back({
            {"id", r.id},
            {"name", r.name},
            {"description", r.description},
            {"factory_key", r.factory_key},
            {"schema_json", r.schema_json},
            {"enabled", r.enabled},
        });
    }
    return arr.dump();
}

// Десериализует JSON-строку обратно в вектор WorkerRecord.
std::vector<WorkerRecord> deserializeRecords(const std::string& data) {
    const auto arr = json::parse(data);
    std::vector<WorkerRecord> result;
    for (const auto& item : arr) {
        WorkerRecord r;
        r.id = item.at("id").get<int>();
        r.name = item.at("name").get<std::string>();
        r.description = item.at("description").get<std::string>();
        r.factory_key = item.at("factory_key").get<std::string>();
        r.schema_json = item.at("schema_json").get<std::string>();
        r.enabled = item.at("enabled").get<bool>();
        result.push_back(std::move(r));
    }
    return result;
}

}  // namespace

WorkerRepository::WorkerRepository(
    const std::string& connection_string,
    RedisCache& cache,
    bool cache_enabled
)
    : connection_(connection_string)
    , cache_(cache)
    , cache_enabled_(cache_enabled)
{
    if (!connection_.is_open()) {
        throw std::runtime_error("Cannot open PostgreSQL connection");
    }
}

std::vector<WorkerRecord> WorkerRepository::findAllEnabled() const {
    // 1. Пробуем кэш
    if (cache_enabled_) {
        if (const auto cached = cache_.get(kCacheKey); cached.has_value()) {
            try {
                return deserializeRecords(cached.value());
            } catch (const std::exception&) {
                // Битый кэш — игнорируем, идём в PostgreSQL
            }
        }
    }

    // 2. Идём в PostgreSQL
    std::vector<WorkerRecord> result;

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

    // 3. Кладём в кэш (best effort — если Redis упал, не роняем запрос)
    if (cache_enabled_) {
        try {
            cache_.set(kCacheKey, serializeRecords(result), kCacheTtlSeconds);
        } catch (const std::exception&) {
            // Redis недоступен — работаем без кэша
        }
    }

    return result;
}
