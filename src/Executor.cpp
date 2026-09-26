#include "sader/Executor.h"

#include <iostream>
#include <stdexcept>
#include <utility>
#include <variant>

void Executor::addWorker(std::unique_ptr<Worker> worker) {
    workers_.push_back(std::move(worker));
}

const Worker* Executor::findWorker(std::string_view name) const {
    for (const auto& worker : workers_) {
        if (worker->name() == name) {
            return worker.get();
        }
    }
    return nullptr;
}

void Executor::discover(std::string_view query) const {
    bool found = false;

    for (const auto& worker : workers_) {
        const std::string_view description = worker->description();

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

void Executor::describe(std::string_view worker_name) const {
    const Worker* worker = findWorker(worker_name);
    if (worker == nullptr) {
        throw std::runtime_error("Worker not found: " + std::string(worker_name));
    }

    std::cout << "name: " << worker->name() << '\n';
    std::cout << "description: " << worker->description() << '\n';

    const Schema schema = worker->schema();

    if (schema.arguments.empty()) {
        std::cout << "arguments: (none)\n";
        return;
    }

    std::cout << "arguments:\n";
    for (const auto& arg : schema.arguments) {
        std::cout
            << "  " << arg.name
            << " (" << arg.type;
        if (arg.required) {
            std::cout << ", required";
        } else {
            std::cout << ", optional";
        }
        std::cout << ") - " << arg.description << '\n';
    }
}

void Executor::call(const CallCommand& cmd) const {
    const Worker* worker = findWorker(cmd.worker_name);
    if (worker == nullptr) {
        throw std::runtime_error("Worker not found: " + cmd.worker_name);
    }

    // Собираем Arguments из CallCommand.
    // В cmd.args уже лежат пары key -> value,
    // их надо просто скопировать в Arguments.values.
    Arguments args;
    args.values = cmd.args;

    // Если у пользователя есть operation, добавляем её в args.
    // TextWorker ждёт "operation" в Arguments.
    // (Позже, когда сделаем других Worker'ов, это может поменяться.)
    args.values["operation"] = cmd.operation;

    // Вызываем execute. Он возвращает Result.
    const Result result = worker->execute(args);

    // Печатаем результат.
    if (result.success) {
        std::cout << result.value << '\n';
    } else {
        std::cerr << "ERROR: " << result.error << '\n';
    }
}

void Executor::execute(const Command& command) const {
    if (const auto* d = std::get_if<DiscoverCommand>(&command)) {
        discover(d->query);
        return;
    }

    if (const auto* d = std::get_if<DescribeCommand>(&command)) {
        describe(d->worker_name);
        return;
    }

    if (const auto* c = std::get_if<CallCommand>(&command)) {
        call(*c);
        return;
    }
}
