#pragma once

#include <map>
#include <string>

// Arguments — значения аргументов, которые передал пользователь.
//
// std::map<std::string, std::string> — это словарь:
// ключ (имя аргумента) -> значение (в виде строки).
//
// Пример: после CALL text length --text=hello
// values = { "text": "hello" }
//
// Почему значения — строки? Потому что пользователь вводит текст.
// Преобразование в int/bool делает Worker внутри execute(),
// и там же проверяет, что преобразование корректно.
struct Arguments {
    std::map<std::string, std::string> values;
};
