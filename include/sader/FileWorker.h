#pragma once

#include "Worker.h"

class FileWorker final : public Worker {
public:
    std::string_view name() const noexcept override;
    std::string_view description() const noexcept override;
};
