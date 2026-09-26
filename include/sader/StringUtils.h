#pragma once

#include <string_view>
#include <vector>

// Разбивает строку по заданному разделителю.
// Например: split("a b c", ' ') -> ["a", "b", "c"]
// Пустые токены (от подряд идущих разделителей) пропускаются.
std::vector<std::string_view> split(std::string_view line, char delimiter);

// Разбирает токен вида "--key=value".
// Возвращает пару (key, value), где key — без префикса "--".
// Если токен не в формате "--key=value" — бросает std::invalid_argument.
std::pair<std::string_view, std::string_view> parse_key_value(std::string_view token);
