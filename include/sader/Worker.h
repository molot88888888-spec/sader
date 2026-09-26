#pragma once

#include "Arguments.h"
#include "Result.h"
#include "Schema.h"

#include <string_view>

// Worker — абстрактный базовый класс для всех инструментов SADER.
//
// Контракт Worker'а:
//   - name()        — короткое имя ("hash", "text", ...)
//   - description() — текстовое описание для DISCOVER
//   - schema()      — какие аргументы принимает (для DESCRIBE)
//   - execute()     — выполнить операцию (для CALL)
class Worker {
public:
    virtual std::string_view name() const noexcept = 0;
    virtual std::string_view description() const noexcept = 0;

    // Возвращаем Schema по значению: это маленькая структура,
    // копировать её дешевле, чем возиться с указателями/lifetime.
    virtual Schema schema() const = 0;

    // execute — выполняет операцию с переданными аргументами.
    // Принимаем Arguments по const&, потому что не хотим копировать
    // и не хотим менять аргументы. Возвращаем Result — структуру,
    // которая говорит, успех это или ошибка.
    virtual Result execute(const Arguments& args) const = 0;

    virtual ~Worker() = default;
};
