#include "sader/HashWorker.h"

std::string_view HashWorker::name() const noexcept {
    return "hash";
}

std::string_view HashWorker::description() const noexcept {
    return "Calculate md5 and sha256 hashes for text and files";
}
