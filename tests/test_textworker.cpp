#include "sader/Arguments.h"
#include "sader/Result.h"
#include "sader/TextWorker.h"

#include <iostream>
#include <string>

// Простой счётчик пройденных и проваленных проверок.
static int g_passed = 0;
static int g_failed = 0;

// Проверяет условие и печатает PASS/FAIL.
// Если condition == true — PASS, иначе FAIL с сообщением.
static void check(bool condition, const std::string& name) {
    if (condition) {
        std::cout << "PASS: " << name << '\n';
        ++g_passed;
    } else {
        std::cout << "FAIL: " << name << '\n';
        ++g_failed;
    }
}

// Проверяет, что строка `haystack` содержит подстроку `needle`.
static void check_contains(const std::string& haystack,
                           const std::string& needle,
                           const std::string& name) {
    check(haystack.find(needle) != std::string::npos, name);
}

int main() {
    TextWorker worker;

    // ---------- Тест 1: успешный вызов length ----------
    {
        Arguments args;
        args.values["operation"] = "length";
        args.values["text"] = "hello";

        const Result r = worker.execute(args);

        check(r.success, "length: success == true");
        check(r.value == "5", "length: value == \"5\"");
    }

    // ---------- Тест 2: успешный вызов words ----------
    {
        Arguments args;
        args.values["operation"] = "words";
        args.values["text"] = "hello world foo";

        const Result r = worker.execute(args);

        check(r.success, "words: success == true");
        check(r.value == "3", "words: value == \"3\"");
    }

    // ---------- Тест 3: ошибка — нет operation ----------
    {
        Arguments args;
        args.values["text"] = "hello";

        const Result r = worker.execute(args);

        check(!r.success, "missing operation: success == false");
        check_contains(r.error, "operation", "missing operation: error mentions 'operation'");
    }

    // ---------- Тест 4: ошибка — нет text ----------
    {
        Arguments args;
        args.values["operation"] = "length";

        const Result r = worker.execute(args);

        check(!r.success, "missing text: success == false");
        check_contains(r.error, "text", "missing text: error mentions 'text'");
    }

    // ---------- Тест 5: ошибка — неизвестная operation ----------
    {
        Arguments args;
        args.values["operation"] = "banana";
        args.values["text"] = "hello";

        const Result r = worker.execute(args);

        check(!r.success, "unknown operation: success == false");
        check_contains(r.error, "Unknown", "unknown operation: error mentions 'Unknown'");
    }

    // ---------- Итог ----------
    std::cout << "\nTotal: " << g_passed << " passed, "
              << g_failed << " failed\n";

    // Возвращаем 0, если всё прошло, 1 — если были провалы.
    // Это стандартный способ сказать «тест прошёл/провалился».
    return g_failed == 0 ? 0 : 1;
}
