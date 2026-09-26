#include "sader/Arguments.h"
#include "sader/FileWorker.h"
#include "sader/Result.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

static int g_passed = 0;
static int g_failed = 0;

static void check(bool condition, const std::string& name) {
    if (condition) {
        std::cout << "PASS: " << name << '\n';
        ++g_passed;
    } else {
        std::cout << "FAIL: " << name << '\n';
        ++g_failed;
    }
}

int main() {
    // Подготовка: создаём временный файл с известным содержимым.
    const fs::path temp_dir = fs::temp_directory_path() / "sader_test";
    fs::create_directories(temp_dir);

    const fs::path test_file = temp_dir / "test.txt";
    const std::string test_content = "hello from file";

    {
        std::ofstream out(test_file);
        out << test_content;
    }   // RAII: out закрывается автоматически здесь

    const fs::path missing_file = temp_dir / "does_not_exist.txt";
    // На случай, если файл остался с прошлого запуска — удаляем.
    fs::remove(missing_file);

    FileWorker worker;

    // ---------- exists: true ----------
    {
        Arguments args;
        args.values["operation"] = "exists";
        args.values["path"] = test_file.string();

        const Result r = worker.execute(args);
        check(r.success, "exists existing: success");
        check(r.value == "true", "exists existing: value == \"true\"");
    }

    // ---------- exists: false ----------
    {
        Arguments args;
        args.values["operation"] = "exists";
        args.values["path"] = missing_file.string();

        const Result r = worker.execute(args);
        check(r.success, "exists missing: success");
        check(r.value == "false", "exists missing: value == \"false\"");
    }

    // ---------- size ----------
    {
        Arguments args;
        args.values["operation"] = "size";
        args.values["path"] = test_file.string();

        const Result r = worker.execute(args);
        check(r.success, "size: success");
        check(r.value == std::to_string(test_content.size()),
              "size: value matches content length");
    }

    // ---------- read ----------
    {
        Arguments args;
        args.values["operation"] = "read";
        args.values["path"] = test_file.string();

        const Result r = worker.execute(args);
        check(r.success, "read: success");
        check(r.value == test_content, "read: content matches");
    }

    // ---------- ошибка: нет operation ----------
    {
        Arguments args;
        args.values["path"] = test_file.string();

        const Result r = worker.execute(args);
        check(!r.success, "missing operation: fail");
    }

    // ---------- ошибка: нет path ----------
    {
        Arguments args;
        args.values["operation"] = "read";

        const Result r = worker.execute(args);
        check(!r.success, "missing path: fail");
    }

    // ---------- ошибка: пустой path ----------
    {
        Arguments args;
        args.values["operation"] = "read";
        args.values["path"] = "";

        const Result r = worker.execute(args);
        check(!r.success, "empty path: fail");
    }

    // ---------- ошибка: read несуществующего ----------
    {
        Arguments args;
        args.values["operation"] = "read";
        args.values["path"] = missing_file.string();

        const Result r = worker.execute(args);
        check(!r.success, "read missing: fail");
    }

    // ---------- ошибка: неизвестная операция ----------
    {
        Arguments args;
        args.values["operation"] = "banana";
        args.values["path"] = test_file.string();

        const Result r = worker.execute(args);
        check(!r.success, "unknown operation: fail");
    }

    // Уборка: удаляем временный файл.
    fs::remove(test_file);
    fs::remove(temp_dir);

    std::cout << "\nTotal: " << g_passed << " passed, "
              << g_failed << " failed\n";

    return g_failed == 0 ? 0 : 1;
}
