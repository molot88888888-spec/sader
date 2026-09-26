#include "sader/FileWorker.h"

std::string_view FileWorker::name() const noexcept {
    return "file";
}

std::string_view FileWorker::description() const noexcept {
    return "Read text files and return file content and metadata";
}

Schema FileWorker::schema() const {
    return Schema{};
}

Result FileWorker::execute(const Arguments& /*args*/) const {
    return Result::fail("FileWorker::execute not implemented yet");
}
