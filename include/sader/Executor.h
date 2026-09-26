#pragma once

#include "Command.h"
#include "Worker.h"

#include <memory>
#include <string_view>
#include <vector>

// Executor — мозг SADER.
// Хранит Worker'ов, выполняет команды.
class Executor {
public:
    // Принимает Worker по unique_ptr — то есть забирает владение.
    // После вызова addWorker вызывающий больше не владеет объектом.
    void addWorker(std::unique_ptr<Worker> worker);

    // Выполняет уже распарсенную команду.
    // const — потому что не меняет состояние Executor.
    void execute(const Command& command) const;

private:
    // Внутренний метод для DISCOVER. Снаружи не виден — деталь реализации.
    void discover(std::string_view query) const;

    // Executor владеет набором Worker'ов.
    // vector<unique_ptr<Worker>> — каждый Worker живёт, пока жив Executor.
    std::vector<std::unique_ptr<Worker>> workers_;
};
