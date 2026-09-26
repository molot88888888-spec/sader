#include "sader/HttpWorker.h"

#include <curl/curl.h>

#include <memory>
#include <stdexcept>
#include <string>

namespace {

// --- RAII над CURL* ---
// curl_easy_init() создаёт handle, curl_easy_cleanup() освобождает.
// Оборачиваем в unique_ptr с кастомным deleter.

struct CurlDeleter {
    void operator()(CURL* handle) const noexcept {
        if (handle != nullptr) {
            curl_easy_cleanup(handle);
        }
    }
};

using CurlPtr = std::unique_ptr<CURL, CurlDeleter>;

// --- Глобальная инициализация libcurl ---
// curl_global_init() надо вызвать один раз за всю программу.
// Делаем через статический объект: конструктор при первом использовании,
// деструктор при выходе из программы. RAII для глобального состояния.

struct CurlGlobal {
    CurlGlobal() {
        if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
            throw std::runtime_error("curl_global_init failed");
        }
    }
    ~CurlGlobal() {
        curl_global_cleanup();
    }
};

void ensureCurlGlobalInit() {
    static CurlGlobal instance;  // инициализируется при первом вызове
    (void)instance;
}

// Callback для записи полученных данных в std::string.
// Сигнатура фиксирована libcurl: (char* data, size_t size, size_t nmemb, void* userp)
// size * nmemb = количество байт в этом chunk'е.
// userp — то, что мы передали через CURLOPT_WRITEDATA.
std::size_t writeCallback(char* ptr, std::size_t size, std::size_t nmemb, void* userp) {
    const std::size_t total = size * nmemb;
    auto* out = static_cast<std::string*>(userp);
    out->append(ptr, total);
    return total;  // сколько байт «приняли»; если меньше — curl прервёт запрос
}

// Проверяет, что URL начинается с http:// или https://
void validateUrl(const std::string& url) {
    constexpr std::string_view http  = "http://";
    constexpr std::string_view https = "https://";

    if (!url.starts_with(http) && !url.starts_with(https)) {
        throw std::runtime_error("URL must start with http:// or https://");
    }
}

// Максимальный размер ответа: 1 МБ
constexpr std::size_t kMaxBodySize = 1024 * 1024;

// Таймаут в секундах
constexpr long kTimeoutSeconds = 10;

// Результат HTTP-запроса: body + status code.
struct HttpResult {
    std::string body;
    long status_code = 0;
};

// Выполняет GET-запрос. Бросает std::runtime_error при ошибках.
HttpResult httpGet(const std::string& url) {
    ensureCurlGlobalInit();

    CURL* raw = curl_easy_init();
    if (raw == nullptr) {
        throw std::runtime_error("curl_easy_init failed");
    }
    CurlPtr handle(raw);

    HttpResult result;

    // Устанавливаем опции. Каждая возвращает CURLcode — проверяем.
    auto setopt = [&](CURLoption opt, auto value) {
        if (curl_easy_setopt(handle.get(), opt, value) != CURLE_OK) {
            throw std::runtime_error("curl_easy_setopt failed");
        }
    };

    setopt(CURLOPT_URL, url.c_str());
    setopt(CURLOPT_WRITEFUNCTION, writeCallback);
    setopt(CURLOPT_WRITEDATA, &result.body);
    setopt(CURLOPT_TIMEOUT, kTimeoutSeconds);
    setopt(CURLOPT_FOLLOWLOCATION, 1L);        // следовать redirect'ам
    setopt(CURLOPT_MAXFILESIZE_LARGE, static_cast<curl_off_t>(kMaxBodySize));
    setopt(CURLOPT_USERAGENT, "sader/0.1");

    const CURLcode code = curl_easy_perform(handle.get());
    if (code != CURLE_OK) {
        throw std::runtime_error(
            std::string("curl_easy_perform failed: ") + curl_easy_strerror(code)
        );
    }

    // Получаем HTTP status code.
    if (curl_easy_getinfo(handle.get(), CURLINFO_RESPONSE_CODE, &result.status_code) != CURLE_OK) {
        throw std::runtime_error("curl_easy_getinfo failed");
    }

    // Проверяем, что не превысили лимит (на случай, если MAXFILESIZE не сработал)
    if (result.body.size() > kMaxBodySize) {
        throw std::runtime_error("response too large");
    }

    return result;
}

}  // namespace

std::string_view HttpWorker::name() const noexcept {
    return "http";
}

std::string_view HttpWorker::description() const noexcept {
    return "HTTP GET requests: fetch body or status code from an http(s) URL";
}

Schema HttpWorker::schema() const {
    return Schema{
        {
            {"operation", "string", true, "Operation: get | status"},
            {"url",       "string", true, "URL starting with http:// or https://"},
        }
    };
}

Result HttpWorker::execute(const Arguments& args) const {
    const auto op_it = args.values.find("operation");
    if (op_it == args.values.end()) {
        return Result::fail("Missing required argument: operation");
    }
    const std::string& operation = op_it->second;

    const auto url_it = args.values.find("url");
    if (url_it == args.values.end()) {
        return Result::fail("Missing required argument: url");
    }

    // Валидация URL — до любых сетевых вызовов.
    try {
        validateUrl(url_it->second);
    } catch (const std::exception& e) {
        return Result::fail(e.what());
    }

    if (operation != "get" && operation != "status") {
        return Result::fail("Unknown operation: " + operation);
    }

    // Выполняем запрос — все ошибки сети/curl превращаем в Result::fail.
    try {
        const HttpResult res = httpGet(url_it->second);

        if (operation == "status") {
            return Result::ok(std::to_string(res.status_code));
        }

        // get
        return Result::ok(res.body);
    } catch (const std::exception& e) {
        return Result::fail(std::string("HTTP request failed: ") + e.what());
    }
}
