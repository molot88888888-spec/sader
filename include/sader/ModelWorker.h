#pragma once

#include "Worker.h"

#include <string>
#include <vector>

class ModelWorker final : public Worker {
public:
    std::string_view name() const noexcept override;
    std::string_view description() const noexcept override;
    Schema schema() const override;
    Result execute(const Arguments& args) const override;

private:
    // L2-нормализация вектора (unit vector).
    // Возвращает Result::ok со строкой "v1,v2,..." или Result::fail с описанием ошибки.
    Result normalize(const std::vector<double>& v) const;
};
