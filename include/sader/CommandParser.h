#pragma once

#include "Command.h"

#include <string_view>

// Разбирает строку "DISCOVER <query>" в Command.
// Бросает std::invalid_argument, если команда не распознана
// или аргумент пустой.
Command parseCommand(std::string_view line);
