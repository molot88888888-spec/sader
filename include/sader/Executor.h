#pragma once

#include "Command.h"
#include "Worker.h"

#include <memory>
#include <string_view>
#include <vector>

class Executor {
public:
    void addWorker(std::unique_ptr<Worker> worker);
    void execute(const Command& command) const;

private:
    void discover(std::string_view query) const;
    void describe(std::string_view worker_name) const;
    void call(const CallCommand& cmd) const;

    // Находит Worker по точному имени.
    // Возвращает nullptr, если не найден.
    const Worker* findWorker(std::string_view name) const;

    std::vector<std::unique_ptr<Worker>> workers_;
};
