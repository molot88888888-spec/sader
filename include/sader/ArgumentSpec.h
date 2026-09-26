#pragma once

#include <string>

// Описание одного аргумента Worker'а.
// Используется в Schema, чтобы объяснить пользователю (или AI-агенту),
// какие аргументы принимает Worker.
struct ArgumentSpec {
    std::string name;         // "text", "algorithm"
    std::string type;         // "string", "int", "bool"
    bool required;            // обязателен ли
    std::string description;  // человекочитаемое описание
};
