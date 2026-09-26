#include "sader/CommandParser.h"
#include "sader/Executor.h"
#include "sader/FileWorker.h"
#include "sader/HashWorker.h"

#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include "sader/TextWorker.h"
#include "sader/ModelWorker.h"
#include "sader/CsvWorker.h"
#include "sader/JsonWorker.h"

int main() {
    // Создаём Executor и регистрируем Worker'ов.
    // make_unique создаёт объект в куче и сразу оборачивает в unique_ptr.
    Executor executor;
    executor.addWorker(std::make_unique<HashWorker>());
    executor.addWorker(std::make_unique<FileWorker>());
    executor.addWorker(std::make_unique<TextWorker>());
    executor.addWorker(std::make_unique<ModelWorker>());
    executor.addWorker(std::make_unique<CsvWorker>());
    executor.addWorker(std::make_unique<JsonWorker>());

    std::string line;

    while (true) {
        // Приглашение к вводу
        std::cout << "SADER> ";

        // getline читает всю строку, включая пробелы
        if (!std::getline(std::cin, line)) {
            break;  // Ctrl+D — выходим
        }

        try {
            const Command command = parseCommand(line);
            executor.execute(command);
        } catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << '\n';
        }
    }

    return 0;
}
