#include "sader/StringUtils.h"

#include <stdexcept>

std::vector<std::string_view> split(std::string_view line, char delimiter) {
    std::vector<std::string_view> tokens;

    std::size_t start = 0;
    while (start < line.size()) {
        // Ищем разделитель, начиная с позиции start
        const std::size_t end = line.find(delimiter, start);

        if (end == std::string_view::npos) {
            // Разделителя больше нет — берём остаток строки
            tokens.push_back(line.substr(start));
            break;
        }

        // Берём подстроку между start и end
        if (end > start) {   // пропускаем пустые токены
            tokens.push_back(line.substr(start, end - start));
        }

        start = end + 1;  // перепрыгиваем через разделитель
    }

    return tokens;
}

std::pair<std::string_view, std::string_view> parse_key_value(std::string_view token) {
    constexpr std::string_view prefix = "--";

    if (!token.starts_with(prefix)) {
        throw std::invalid_argument(
            "Argument must start with '--': " + std::string(token)
        );
    }

    const std::size_t eq = token.find('=');
    if (eq == std::string_view::npos) {
        throw std::invalid_argument(
            "Argument must be in format --key=value: " + std::string(token)
        );
    }

    // key — между "--" и "="
    const std::string_view key = token.substr(prefix.size(), eq - prefix.size());
    // value — после "="
    const std::string_view value = token.substr(eq + 1);

    if (key.empty()) {
        throw std::invalid_argument("Argument key is empty: " + std::string(token));
    }

    return {key, value};
}
