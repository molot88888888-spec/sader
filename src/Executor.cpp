#include "sader/Executor.h"

#include <iostream>
#include <stdexcept>
#include <utility>
#include <variant>

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
    // Пробуем достать DiscoverCommand из variant'а.
    // Если внутри именно он — get_if вернёт указатель, иначе nullptr.
    if (const auto* d = std::get_if<DiscoverCommand>(&command)) {
        discover(d->query);
        return;
    }

    if (const auto* d = std::get_if<DescribeCommand>(&command)) {
        // Пока не реализовано — заглушка. Реализуем на 2.4.3.
        (void)d;  // подавляем warning об неиспользуемой переменной
        throw std::runtime_error("DESCRIBE not implemented yet");
    }

    if (const auto* c = std::get_if<CallCommand>(&command)) {
        // Пока не реализовано — заглушка.
        (void)c;
        throw std::runtime_error("CALL not implemented yet");
    }
}
