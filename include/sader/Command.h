#pragma once

#include <string>

// Тип команды, которую ввёл пользователь.
// enum class (а не просто enum) — значения не «протекают» наружу
// как int'ы, их нельзя случайно сравнить с числом.
enum class CommandType {
    Discover,
    Describe,
    Call
};

// Command — внутреннее представление команды SADER.
// Это просто значение: struct с двумя полями, никаких ресурсов,
// никаких ручных деструкторов. Rule of Zero.
struct Command {
    CommandType type;
    std::string argument;  // std::string, потому что Command владеет аргументом
};
