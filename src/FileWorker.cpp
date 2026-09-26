#include "sader/FileWorker.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

namespace {
    // Максимальный размер файла для операции read: 1 МБ.
    // Защита от случайного чтения гигабайтов.
    constexpr std::uintmax_t kMaxReadSize = 1024 * 1024;
}

std::string_view FileWorker::name() const noexcept {
    return "file";
}

std::string_view FileWorker::description() const noexcept {
    return "File operations: read content, get size, check existence";
}

Schema FileWorker::schema() const {
    return Schema{
        {
            {"operation", "string", true, "Operation: read | size | exists"},
            {"path",      "string", true, "Path to the file"},
        }
    };
}

Result FileWorker::execute(const Arguments& args) const {
    // 1. Проверяем обязательные аргументы.
    const auto op_it = args.values.find("operation");
    if (op_it == args.values.end()) {
        return Result::fail("Missing required argument: operation");
    }

    const auto path_it = args.values.find("path");
    if (path_it == args.values.end()) {
        return Result::fail("Missing required argument: path");
    }

    const std::string& operation = op_it->second;
    const std::string& path = path_it->second;

    if (path.empty()) {
        return Result::fail("Argument 'path' must not be empty");
    }

    // 2. exists
    if (operation == "exists") {
        // fs::exists может бросить исключение при проблемах с правами доступа.
        // Ловим и превращаем в Result::fail — это ожидаемая ошибка.
        std::error_code ec;
        const bool exists = fs::exists(path, ec);

        if (ec) {
            return Result::fail("Cannot check file: " + ec.message());
        }

        return Result::ok(exists ? "true" : "false");
    }

    // 3. size
    if (operation == "size") {
        std::error_code ec;
        if (!fs::exists(path, ec)) {
            if (ec) {
                return Result::fail("Cannot check file: " + ec.message());
            }
            return Result::fail("File does not exist: " + path);
        }

        const std::uintmax_t size = fs::file_size(path, ec);
        if (ec) {
            return Result::fail("Cannot get file size: " + ec.message());
        }

        return Result::ok(std::to_string(size));
    }

    // 4. read
    if (operation == "read") {
        std::error_code ec;
        if (!fs::exists(path, ec)) {
            if (ec) {
                return Result::fail("Cannot check file: " + ec.message());
            }
            return Result::fail("File does not exist: " + path);
        }

        const std::uintmax_t size = fs::file_size(path, ec);
        if (ec) {
            return Result::fail("Cannot get file size: " + ec.message());
        }

        if (size > kMaxReadSize) {
            return Result::fail(
                "File is too large: " + std::to_string(size) +
                " bytes (max " + std::to_string(kMaxReadSize) + ")"
            );
        }

        // std::ifstream — RAII. При выходе из функции (любым путём,
        // включая исключение) деструктор автоматически закроет файл.
        std::ifstream file(path);
        if (!file) {
            return Result::fail("Cannot open file for reading: " + path);
        }

        std::ostringstream buffer;
        buffer << file.rdbuf();

        if (file.bad()) {
            return Result::fail("I/O error while reading file: " + path);
        }

        return Result::ok(buffer.str());
    }

    return Result::fail("Unknown operation: " + operation);
}
