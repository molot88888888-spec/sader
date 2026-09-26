#pragma once

#include "Worker.h"

// TextWorker — операции над текстом: length, words, lines.
class TextWorker final : public Worker {
public:
    std::string_view name() const noexcept override;
    std::string_view description() const noexcept override;
    Schema schema() const override;
    Result execute(const Arguments& args) const override;
};
