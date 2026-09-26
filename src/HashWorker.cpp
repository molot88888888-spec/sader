#include "sader/HashWorker.h"

#include <openssl/evp.h>

#include <array>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>

namespace {

// --- RAII-обёртка вокруг EVP_MD_CTX ---
//
// EVP_MD_CTX — это C-структура. Её надо создавать через EVP_MD_CTX_new()
// и освобождать через EVP_MD_CTX_free(). Если забыть освободить —
// утечка. Если исключение в середине работы — тоже утечка.
//
// Решение: обернуть в умный указатель с кастомным deleter'ом.
// Deleter — это объект, который знает, как «удалять» объект.
// unique_ptr вызовет его автоматически в деструкторе.

struct EvpMdCtxDeleter {
    void operator()(EVP_MD_CTX* ctx) const noexcept {
        if (ctx != nullptr) {
            EVP_MD_CTX_free(ctx);
        }
    }
};

using EvpMdCtxPtr = std::unique_ptr<EVP_MD_CTX, EvpMdCtxDeleter>;

// Создаёт контекст или бросает std::runtime_error.
EvpMdCtxPtr makeEvpContext() {
    EVP_MD_CTX* raw = EVP_MD_CTX_new();
    if (raw == nullptr) {
        throw std::runtime_error("EVP_MD_CTX_new failed");
    }
    return EvpMdCtxPtr(raw);
}

// Преобразует байты в hex-строку: 0xAB -> "ab"
std::string toHex(const unsigned char* data, std::size_t length) {
    static constexpr char digits[] = "0123456789abcdef";
    std::string result;
    result.reserve(length * 2);

    for (std::size_t i = 0; i < length; ++i) {
        result.push_back(digits[(data[i] >> 4) & 0x0F]);
        result.push_back(digits[data[i] & 0x0F]);
    }
    return result;
}

// Вычисляет хеш строки с заданным алгоритмом.
// algorithm — это EVP_sha256(), EVP_md5() и т. п.
std::string computeHash(const EVP_MD* algorithm, const std::string& text) {
    EvpMdCtxPtr ctx = makeEvpContext();

    if (EVP_DigestInit_ex(ctx.get(), algorithm, nullptr) != 1) {
        throw std::runtime_error("EVP_DigestInit_ex failed");
    }

    if (EVP_DigestUpdate(ctx.get(), text.data(), text.size()) != 1) {
        throw std::runtime_error("EVP_DigestUpdate failed");
    }

    std::array<unsigned char, EVP_MAX_MD_SIZE> buffer{};
    unsigned int out_length = 0;

    if (EVP_DigestFinal_ex(ctx.get(), buffer.data(), &out_length) != 1) {
        throw std::runtime_error("EVP_DigestFinal_ex failed");
    }

    return toHex(buffer.data(), out_length);
}

}  // namespace

std::string_view HashWorker::name() const noexcept {
    return "hash";
}

std::string_view HashWorker::description() const noexcept {
    return "Calculate md5 and sha256 hashes for text";
}

Schema HashWorker::schema() const {
    return Schema{
        {
            {"operation", "string", true, "Hash algorithm: md5 | sha256"},
            {"text",      "string", true, "Input text to hash"},
        }
    };
}

Result HashWorker::execute(const Arguments& args) const {
    const auto op_it = args.values.find("operation");
    if (op_it == args.values.end()) {
        return Result::fail("Missing required argument: operation");
    }

    const auto text_it = args.values.find("text");
    if (text_it == args.values.end()) {
        return Result::fail("Missing required argument: text");
    }

    const std::string& algorithm = op_it->second;
    const std::string& text = text_it->second;

    // Выбираем алгоритм.
    const EVP_MD* md = nullptr;
    if (algorithm == "md5") {
        md = EVP_md5();
    } else if (algorithm == "sha256") {
        md = EVP_sha256();
    } else {
        return Result::fail("Unknown algorithm: " + algorithm);
    }

    // OpenSSL может бросить std::runtime_error из computeHash.
    // Ловим и превращаем в Result::fail — потому что для нас
    // это ожидаемая ошибка выполнения операции.
    try {
        const std::string hash = computeHash(md, text);
        return Result::ok(hash);
    } catch (const std::exception& e) {
        return Result::fail(std::string("Hash computation failed: ") + e.what());
    }
}
