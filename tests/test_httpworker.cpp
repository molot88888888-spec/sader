#include "sader/Arguments.h"
#include "sader/HttpWorker.h"
#include "sader/Result.h"

#include <iostream>
#include <string>

static int g_passed = 0;
static int g_failed = 0;

static void check(bool condition, const std::string& name) {
    if (condition) { std::cout << "PASS: " << name << '\n'; ++g_passed; }
    else { std::cout << "FAIL: " << name << '\n'; ++g_failed; }
}

static void check_contains(const std::string& haystack,
                           const std::string& needle,
                           const std::string& name) {
    check(haystack.find(needle) != std::string::npos, name);
}

int main() {
    HttpWorker worker;

    // ---- Валидация (не требует сети) ----

    {
        Arguments args;
        args.values["operation"] = "get";
        args.values["url"] = "ftp://example.com";
        const Result r = worker.execute(args);
        check(!r.success, "ftp url: fail");
        check_contains(r.error, "http", "ftp url: error mentions http");
    }

    {
        Arguments args;
        args.values["operation"] = "get";
        args.values["url"] = "example.com";
        const Result r = worker.execute(args);
        check(!r.success, "no scheme: fail");
    }

    {
        Arguments args;
        args.values["operation"] = "get";
        args.values["url"] = "";
        const Result r = worker.execute(args);
        check(!r.success, "empty url: fail");
    }

    {
        Arguments args;
        args.values["operation"] = "banana";
        args.values["url"] = "https://example.com";
        const Result r = worker.execute(args);
        check(!r.success, "unknown operation: fail");
    }

    {
        Arguments args;
        args.values["operation"] = "get";
        const Result r = worker.execute(args);
        check(!r.success, "missing url: fail");
    }

    // ---- Реальные запросы (требуют интернета) ----
    // Если сети нет — эти тесты упадут. Можно закомментировать,
    // если работаешь офлайн.

    {
        Arguments args;
        args.values["operation"] = "status";
        args.values["url"] = "https://example.com";
        const Result r = worker.execute(args);
        check(r.success, "status example.com: success");
        check(r.value == "200", "status example.com: == 200");
    }

    {
        Arguments args;
        args.values["operation"] = "get";
        args.values["url"] = "https://example.com";
        const Result r = worker.execute(args);
        check(r.success, "get example.com: success");
        check_contains(r.value, "Example Domain", "get example.com: contains 'Example Domain'");
    }

    std::cout << "\nTotal: " << g_passed << " passed, "
              << g_failed << " failed\n";

    return g_failed == 0 ? 0 : 1;
}
