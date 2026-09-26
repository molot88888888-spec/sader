#include "sader/Arguments.h"
#include "sader/ProcessWorker.h"
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
    ProcessWorker worker;

    // echo hello
    {
        Arguments args;
        args.values["operation"] = "run";
        args.values["command"] = "echo hello";
        const Result r = worker.execute(args);
        check(r.success, "echo hello: success");
        check_contains(r.value, "hello", "echo hello: contains 'hello'");
    }

    // echo with spaces
    {
        Arguments args;
        args.values["operation"] = "run";
        args.values["command"] = "echo multiple words";
        const Result r = worker.execute(args);
        check(r.success, "echo multiple: success");
        check_contains(r.value, "multiple words", "echo multiple: contains text");
    }

    // whoami
    {
        Arguments args;
        args.values["operation"] = "run";
        args.values["command"] = "whoami";
        const Result r = worker.execute(args);
        check(r.success, "whoami: success");
        check(!r.value.empty(), "whoami: non-empty output");
    }

    // date
    {
        Arguments args;
        args.values["operation"] = "run";
        args.values["command"] = "date";
        const Result r = worker.execute(args);
        check(r.success, "date: success");
        check(!r.value.empty(), "date: non-empty output");
    }

    // Ошибка: команда не в whitelist
    {
        Arguments args;
        args.values["operation"] = "run";
        args.values["command"] = "rm -rf /";
        const Result r = worker.execute(args);
        check(!r.success, "forbidden command: fail");
        check_contains(r.error, "not allowed", "forbidden command: error mentions 'not allowed'");
    }

    // Ошибка: shell injection в echo
    {
        Arguments args;
        args.values["operation"] = "run";
        args.values["command"] = "echo hello; rm -rf /";
        const Result r = worker.execute(args);
        check(!r.success, "injection: fail");
        check_contains(r.error, "forbidden character", "injection: error mentions 'forbidden'");
    }

    // Ошибка: pipe injection
    {
        Arguments args;
        args.values["operation"] = "run";
        args.values["command"] = "echo hello | wc -l";
        const Result r = worker.execute(args);
        check(!r.success, "pipe injection: fail");
    }

    // Ошибка: нет command
    {
        Arguments args;
        args.values["operation"] = "run";
        const Result r = worker.execute(args);
        check(!r.success, "missing command: fail");
    }

    // Ошибка: неизвестная operation
    {
        Arguments args;
        args.values["operation"] = "banana";
        args.values["command"] = "echo hi";
        const Result r = worker.execute(args);
        check(!r.success, "unknown operation: fail");
    }

    std::cout << "\nTotal: " << g_passed << " passed, "
              << g_failed << " failed\n";

    return g_failed == 0 ? 0 : 1;
}
