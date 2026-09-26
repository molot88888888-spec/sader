#include "sader/JsonWorker.h"

#include <nlohmann/json.hpp>

#include <sstream>
#include <stdexcept>
#include <string>

using json = nlohmann::json;

namespace {

// Идёт по пути "a.b.c" в JSON-объекте.
// Бросает std::runtime_error, если ключа нет.
const json& navigate(const json& root, const std::string& path) {
    if (path.empty()) {
        return root;
    }

    const json* current = &root;
    std::istringstream stream(path);
    std::string key;

    while (std::getline(stream, key, '.')) {
        if (key.empty()) {
            throw std::runtime_error("empty key in path");
        }
        if (!current->is_object()) {
            throw std::runtime_error("cannot navigate into non-object at key '" + key + "'");
        }
        if (!current->contains(key)) {
            throw std::runtime_error("key not found: '" + key + "'");
        }
        current = &(*current)[key];
    }

    return *current;
}

std::string typeOf(const json& value) {
    if (value.is_null())    return "null";
    if (value.is_boolean()) return "boolean";
    if (value.is_number())  return "number";
    if (value.is_string())  return "string";
    if (value.is_array())   return "array";
    if (value.is_object())  return "object";
    return "unknown";
}

// Превращает скаляр в строку для вывода.
std::string scalarToString(const json& value) {
    if (value.is_string())  return value.get<std::string>();
    if (value.is_boolean()) return value.get<bool>() ? "true" : "false";
    if (value.is_number())  return value.dump();
    if (value.is_null())    return "null";
    // object/array — возвращаем компактный дамп
    return value.dump();
}

}  // namespace

std::string_view JsonWorker::name() const noexcept {
    return "json";
}

std::string_view JsonWorker::description() const noexcept {
    return "JSON operations: validate, get value by path, get type";
}

Schema JsonWorker::schema() const {
    return Schema{
        {
            {"operation", "string", true, "Operation: parse | get | type"},
            {"json",      "string", true, "JSON content"},
            {"path",      "string", false, "Dot-separated path (required for get/type)"},
        }
    };
}

Result JsonWorker::execute(const Arguments& args) const {
    const auto op_it = args.values.find("operation");
    if (op_it == args.values.end()) {
        return Result::fail("Missing required argument: operation");
    }
    const std::string& operation = op_it->second;

    const auto json_it = args.values.find("json");
    if (json_it == args.values.end()) {
        return Result::fail("Missing required argument: json");
    }

    // Парсим JSON — с обработкой ошибок библиотеки.
    json root;
    try {
        root = json::parse(json_it->second);
    } catch (const json::parse_error& e) {
        return Result::fail(std::string("Invalid JSON: ") + e.what());
    }

    // parse — просто проверить валидность
    if (operation == "parse") {
        return Result::ok("valid");
    }

    // get и type — нужен path
    if (operation == "get" || operation == "type") {
        const auto path_it = args.values.find("path");
        if (path_it == args.values.end()) {
            return Result::fail("Missing required argument: path (needed for " + operation + ")");
        }

        const json* value_ptr = nullptr;
        try {
            value_ptr = &navigate(root, path_it->second);
        } catch (const std::exception& e) {
            return Result::fail(std::string("Navigation failed: ") + e.what());
        }

        if (operation == "type") {
            return Result::ok(typeOf(*value_ptr));
        }

        // get
        return Result::ok(scalarToString(*value_ptr));
    }

    return Result::fail("Unknown operation: " + operation);
}
