#include "sader/HashWorker.h"

std::string_view HashWorker::name() const noexcept {
    return "hash";
}

std::string_view HashWorker::description() const noexcept {
    return "Calculate md5 and sha256 hashes for text and files";
}

// Заглушка: пока схема пустая, execute возвращает ошибку.
// Реализуем по-настоящему на следующем подэтапе.
Schema HashWorker::schema() const {
    return Schema{};
}

Result HashWorker::execute(const Arguments& /*args*/) const {
    return Result::fail("HashWorker::execute not implemented yet");
}
