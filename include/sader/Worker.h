#pragma once

#include <string_view>

// Worker — абстрактный базовый класс для всех инструментов SADER.
// Каждый конкретный Worker (HashWorker, FileWorker, ...) должен
// унаследоваться от него и реализовать name() и description().
class Worker {
public:
    // Имя Worker'а — короткий идентификатор, например "hash" или "file".
    // Возвращаем string_view, потому что это просто литерал — копировать
    // строку каждый раз не нужно.
    virtual std::string_view name() const noexcept = 0;

    // Описание capability — по нему DISCOVER ищет подходящий Worker.
    // Должно содержать ключевые слова, по которым пользователь
    // (или AI-агент) может искать.
    virtual std::string_view description() const noexcept = 0;

    // Виртуальный деструктор ОБЯЗАТЕЛЕН, когда мы удаляем объект
    // производного класса через указатель на базовый (unique_ptr<Worker>).
    // Без него деструктор наследника не вызовется — утечка.
    virtual ~Worker() = default;
};
