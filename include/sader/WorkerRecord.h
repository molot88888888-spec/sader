#pragma once

#include <string>

// WorkerRecord — данные о Worker'е, прочитанные из БД.
// Это НЕ объект Worker (у него нет execute) — только метаданные.
// Executor по factory_key создаст настоящий Worker через фабрику.
struct WorkerRecord {
    int id = 0;
    std::string name;
    std::string description;
    std::string factory_key;
    std::string schema_json;
    bool enabled = true;
};
