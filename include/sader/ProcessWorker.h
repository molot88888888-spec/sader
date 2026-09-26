#pragma once

#include "Worker.h"

class ProcessWorker final : public Worker {
public:
    std::string_view name() const noexcept override;
    std::string_view description() const noexcept override;
    Schema schema() const override;
    Result execute(const Arguments& args) const override;
};
