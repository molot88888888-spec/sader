#include "sader/TextWorker.h"

#include <cstddef>
#include <sstream>
#include <string>

std::string_view TextWorker::name() const noexcept {
    return "text";
}

std::string_view TextWorker::description() const noexcept {
    return "Text operations: count characters, words, or lines in a string";
}

// Описываем, какие аргументы принимает Worker.
// Это то, что покажет DESCRIBE text.
Schema TextWorker::schema() const {
    return Schema{
        {
            {"operation", "string", true, "Operation: length | words | lines"},
            {"text",      "string", true, "Input text to analyze"},
        }
    };
}

Result TextWorker::execute(const Arguments& args) const {
    // 1. Проверяем, что обязательные аргументы вообще есть.
    //    args.values — это std::map<std::string, std::string>.
    //    find() ищет ключ; если не нашёл — возвращает end().
    const auto op_it = args.values.find("operation");
    if (op_it == args.values.end()) {
        return Result::fail("Missing required argument: operation");
    }

    const auto text_it = args.values.find("text");
    if (text_it == args.values.end()) {
        return Result::fail("Missing required argument: text");
    }

    // 2. Извлекаем значения.
    const std::string& operation = op_it->second;
    const std::string& text = text_it->second;

    // 3. Выполняем нужную операцию.
    if (operation == "length") {
        // text.size() возвращает std::size_t (беззнаковое число).
        // Приводим к строке через std::to_string.
        return Result::ok(std::to_string(text.size()));
    }

    if (operation == "words") {
        // std::istringstream — поток, который читает из строки.
        // operator>> читает очередное слово, пропуская пробелы.
        std::istringstream stream(text);
        std::string word;
        std::size_t count = 0;
        while (stream >> word) {
            ++count;
        }
        return Result::ok(std::to_string(count));
    }

    if (operation == "lines") {
        if (text.empty()) {
            return Result::ok("0");
        }

        std::size_t count = 1;
        for (char c : text) {
            if (c == '\n') {
                ++count;
            }
        }
        return Result::ok(std::to_string(count));
    }

    // 4. Неизвестная операция — это тоже ошибка.
    return Result::fail("Unknown operation: " + operation);
}
