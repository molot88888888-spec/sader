#pragma once

#include "Worker.h"

// HashWorker — конкретный Worker, который умеет считать хеши.
// final означает: от этого класса нельзя наследоваться дальше.
class HashWorker final : public Worker {
public:
    std::string_view name() const noexcept override;
    std::string_view description() const noexcept override;
};
