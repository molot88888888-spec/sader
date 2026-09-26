#include "sader/FileWorker.h"

std::string_view FileWorker::name() const noexcept {
    return "file";
}

std::string_view FileWorker::description() const noexcept {
    return "Read text files and return file content and metadata";
}
