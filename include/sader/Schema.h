#pragma once

#include "ArgumentSpec.h"

#include <vector>

// Schema — полный контракт Worker'а.
// Пока это просто список аргументов.
// Позже можно добавить имя операции, версию, тип результата.
struct Schema {
    std::vector<ArgumentSpec> arguments;
};
