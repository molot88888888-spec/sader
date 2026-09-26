#include "sader/Arguments.h"
#include "sader/CsvWorker.h"
#include "sader/Result.h"

#include <iostream>
#include <string>

static int g_passed = 0;
static int g_failed = 0;

static void check(bool condition, const std::string& name) {
    if (condition) { std::cout << "PASS: " << name << '\n'; ++g_passed; }
    else { std::cout << "FAIL: " << name << '\n'; ++g_failed; }
}

int main() {
    CsvWorker worker;

    const std::string csv =
        "name,value\n"
        "a,10\n"
        "b,20\n"
        "c,30\n";

    // count = 3
    {
        Arguments args;
        args.values["operation"] = "count";
        args.values["data"] = csv;
        const Result r = worker.execute(args);
        check(r.success, "count: success");
        check(r.value == "3", "count: value == 3");
    }

    // sum column 1 = 60
    {
        Arguments args;
        args.values["operation"] = "sum";
        args.values["data"] = csv;
        args.values["column"] = "1";
        const Result r = worker.execute(args);
        check(r.success, "sum: success");
        check(r.value == "60.000000", "sum: value == 60");
    }

    // mean column 1 = 20
    {
        Arguments args;
        args.values["operation"] = "mean";
        args.values["data"] = csv;
        args.values["column"] = "1";
        const Result r = worker.execute(args);
        check(r.success, "mean: success");
        check(r.value == "20.000000", "mean: value == 20");
    }

    // min column 1 = 10
    {
        Arguments args;
        args.values["operation"] = "min";
        args.values["data"] = csv;
        args.values["column"] = "1";
        const Result r = worker.execute(args);
        check(r.success, "min: success");
        check(r.value == "10.000000", "min: value == 10");
    }

    // max column 1 = 30
    {
        Arguments args;
        args.values["operation"] = "max";
        args.values["data"] = csv;
        args.values["column"] = "1";
        const Result r = worker.execute(args);
        check(r.success, "max: success");
        check(r.value == "30.000000", "max: value == 30");
    }

    // Ошибка: нет data
    {
        Arguments args;
        args.values["operation"] = "count";
        const Result r = worker.execute(args);
        check(!r.success, "missing data: fail");
    }

    // Ошибка: колонка вне диапазона
    {
        Arguments args;
        args.values["operation"] = "sum";
        args.values["data"] = csv;
        args.values["column"] = "99";
        const Result r = worker.execute(args);
        check(!r.success, "column out of range: fail");
    }

    // Ошибка: невалидное число
    {
        Arguments args;
        args.values["operation"] = "sum";
        args.values["data"] = "name,value\nx,notanumber\n";
        args.values["column"] = "1";
        const Result r = worker.execute(args);
        check(!r.success, "invalid number: fail");
    }

    std::cout << "\nTotal: " << g_passed << " passed, "
              << g_failed << " failed\n";

    return g_failed == 0 ? 0 : 1;
}
