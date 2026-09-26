#include "sader/CommandParser.h"

#include <stdexcept>
#include <string>

Command parseCommand(std::string_view line) {
    // constexpr — вычисляется на этапе компиляции
    constexpr std::string_view discover_prefix = "DISCOVER ";

    if (line.starts_with(discover_prefix)) {
        // substr возвращает string_view на часть после "DISCOVER "
        const std::string_view query = line.substr(discover_prefix.size());

        if (query.empty()) {
            throw std::invalid_argument("DISCOVER requires a query");
        }

        // Возвращаем DiscoverCommand.
        // C++ автоматически превратит его в Command (std::variant),
        // потому что DiscoverCommand — один из вариантов.
        return DiscoverCommand{std::string(query)};
    }

    // Пока поддерживаем только DISCOVER.
    // DESCRIBE и CALL разберём на подэтапе 2.4.2.
    throw std::invalid_argument("Unknown or unsupported command");
}
