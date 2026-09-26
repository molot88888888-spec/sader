#include "sader/Executor.h"

#include <iostream>
#include <stdexcept>
#include <utility>

void Executor::addWorker(std::unique_ptr<Worker> worker) {
    // std::move передаёт владение в vector.
    // После этого локальный параметр worker пустой (nullptr).
    workers_.push_back(std::move(worker));
}

void Executor::discover(std::string_view query) const {
    bool found = false;

    // range-based for: проходим по всем Worker'ам
    // const auto& — не копируем unique_ptr, работаем по ссылке
    for (const auto& worker : workers_) {
        // string_view — не копируем строку описания
        const std::string_view description = worker->description();

        // Ищем query как подстроку в description
        if (description.find(query) != std::string_view::npos) {
            std::cout
                << worker->name()
                << " - "
                << description
                << '\n';

            found = true;
        }
    }

    if (!found) {
        std::cout << "No capabilities found\n";
    }
}

void Executor::execute(const Command& command) const {
    switch (command.type) {
        case CommandType::Discover:
            discover(command.argument);
            return;

        case CommandType::Describe:
        case CommandType::Call:
            throw std::runtime_error("Command is not implemented yet");
    }
}
