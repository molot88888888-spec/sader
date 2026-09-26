#pragma once

#include <string>

// Result — то, что Worker возвращает после execute().
// Это либо успешный результат со значением,
// либо описание ошибки.
//
// Пока простая модель: два поля, одно из которых пустое.
// Позже можно перейти к std::variant, но для начала так нагляднее.
struct Result {
    bool success = false;
    std::string value;   // заполнено, если success == true
    std::string error;   // заполнено, если success == false

    // Удобные фабрики — чтобы не заполнять поля вручную каждый раз.
    static Result ok(std::string value) {
        return Result{true, std::move(value), ""};
    }

    static Result fail(std::string error) {
        return Result{false, "", std::move(error)};
    }
};
