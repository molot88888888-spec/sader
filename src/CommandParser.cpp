#include "sader/CommandParser.h"

#include <stdexcept>
#include <string>

Command parseCommand(std::string_view line) {
    // constexpr — вычисляется на этапе компиляции, без runtime-затрат
    constexpr std::string_view prefix = "DISCOVER ";

    // starts_with — новая функция C++20
    if (!line.starts_with(prefix)) {
        throw std::invalid_argument("Unknown or unsupported command");
    }

    // substr возвращает string_view на часть строки после "DISCOVER "
    const std::string_view query = line.substr(prefix.size());

    if (query.empty()) {
        throw std::invalid_argument("DISCOVER requires a query");
    }

    // Создаём Command. argument — std::string, поэтому явно
    // конструируем его из string_view (копируем данные).
    return {
        CommandType::Discover,
        std::string(query)
    };
}
