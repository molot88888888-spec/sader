#include "sader/ProcessWorker.h"

#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

// --- RAII-обёртка для FILE* из popen ---
// FILE* надо закрывать через pclose, не fclose!
// pclose ещё и ждёт завершения дочернего процесса.

struct PopenFileDeleter {
    void operator()(std::FILE* f) const noexcept {
        if (f != nullptr) {
            pclose(f);
        }
    }
};

using PopenFilePtr = std::unique_ptr<std::FILE, PopenFileDeleter>;

// Открывает команду через popen для чтения.
PopenFilePtr openProcess(const std::string& command) {
    std::FILE* raw = popen(command.c_str(), "r");
    if (raw == nullptr) {
        throw std::runtime_error("popen failed");
    }
    return PopenFilePtr(raw);
}

// Читает всё содержимое FILE* в строку.
std::string readAll(std::FILE* f) {
    std::string result;
    std::array<char, 256> buffer{};
    std::size_t n = 0;
    while ((n = std::fread(buffer.data(), 1, buffer.size(), f)) > 0) {
        result.append(buffer.data(), n);
    }
    return result;
}

// Проверяет, разрешена ли команда, и возвращает её безопасный вариант.
// Разрешены: echo <text>, date, whoami, uname -a
std::string validateCommand(const std::string& command) {
    // Точное совпадение для команд без аргументов.
    if (command == "date" || command == "whoami" || command == "uname -a") {
        return command;
    }

    // echo <text> — проверяем префикс.
    constexpr std::string_view echo_prefix = "echo ";
    if (command.starts_with(echo_prefix)) {
        // Защита от shell-инъекций: запрещаем символы ; | & $ ` < > ( )
        const std::string forbidden = ";|&$`<>()";
        const std::string text = command.substr(echo_prefix.size());
        for (char c : text) {
            if (forbidden.find(c) != std::string::npos) {
                throw std::runtime_error(
                    std::string("forbidden character in echo: '") + c + "'"
                );
            }
        }
        return command;
    }

    throw std::runtime_error("command not allowed: '" + command + "'");
}

}  // namespace

std::string_view ProcessWorker::name() const noexcept {
    return "process";
}

std::string_view ProcessWorker::description() const noexcept {
    return "Run allowed local commands and capture stdout (whitelist: echo, date, whoami, uname)";
}

Schema ProcessWorker::schema() const {
    return Schema{
        {
            {"operation", "string", true, "Operation: run"},
            {"command",   "string", true, "Command from whitelist: echo <text> | date | whoami | uname -a"},
        }
    };
}

Result ProcessWorker::execute(const Arguments& args) const {
    const auto op_it = args.values.find("operation");
    if (op_it == args.values.end()) {
        return Result::fail("Missing required argument: operation");
    }
    if (op_it->second != "run") {
        return Result::fail("Unknown operation: " + op_it->second);
    }

    const auto cmd_it = args.values.find("command");
    if (cmd_it == args.values.end()) {
        return Result::fail("Missing required argument: command");
    }

    // Валидация — только разрешённые команды.
    std::string safe_command;
    try {
        safe_command = validateCommand(cmd_it->second);
    } catch (const std::exception& e) {
        return Result::fail(e.what());
    }

    // Запуск и чтение — всё в try, потому что popen/fread могут бросить.
    try {
        PopenFilePtr pipe = openProcess(safe_command);
        const std::string output = readAll(pipe.get());
        return Result::ok(output);
    } catch (const std::exception& e) {
        return Result::fail(std::string("process failed: ") + e.what());
    }
}
