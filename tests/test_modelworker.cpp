#include "sader/Arguments.h"
#include "sader/ModelWorker.h"
#include "sader/Result.h"

#include <cmath>
#include <iostream>
#include <string>

static int g_passed = 0;
static int g_failed = 0;

static void check(bool condition, const std::string& name) {
    if (condition) { std::cout << "PASS: " << name << '\n'; ++g_passed; }
    else { std::cout << "FAIL: " << name << '\n'; ++g_failed; }
}

static bool approxEqual(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) < eps;
}

int main() {
    ModelWorker worker;

    // normalize([3,4]) = [0.6, 0.8]
    {
        Arguments args;
        args.values["operation"] = "normalize";
        args.values["a"] = "3,4";
        const Result r = worker.execute(args);
        check(r.success, "normalize: success");
        // Проверим, что результат начинается с "0.6" и содержит "0.8"
        check(r.value.find("0.6") != std::string::npos, "normalize: contains 0.6");
        check(r.value.find("0.8") != std::string::npos, "normalize: contains 0.8");
    }

    // dot([1,2,3],[4,5,6]) = 32
    {
        Arguments args;
        args.values["operation"] = "dot";
        args.values["a"] = "1,2,3";
        args.values["b"] = "4,5,6";
        const Result r = worker.execute(args);
        check(r.success, "dot: success");
        check(r.value == "32.000000", "dot: value == 32");
    }

    // cosine([1,0],[0,1]) = 0
    {
        Arguments args;
        args.values["operation"] = "cosine";
        args.values["a"] = "1,0";
        args.values["b"] = "0,1";
        const Result r = worker.execute(args);
        check(r.success, "cosine orthogonal: success");
        check(approxEqual(std::stod(r.value), 0.0), "cosine orthogonal: == 0");
    }

    // cosine([1,1],[1,1]) = 1
    {
        Arguments args;
        args.values["operation"] = "cosine";
        args.values["a"] = "1,1";
        args.values["b"] = "1,1";
        const Result r = worker.execute(args);
        check(r.success, "cosine identical: success");
        check(approxEqual(std::stod(r.value), 1.0), "cosine identical: == 1");
    }

    // Ошибка: нет operation
    {
        Arguments args;
        args.values["a"] = "1,2";
        const Result r = worker.execute(args);
        check(!r.success, "missing operation: fail");
    }

    // Ошибка: невалидное число
    {
        Arguments args;
        args.values["operation"] = "normalize";
        args.values["a"] = "1,abc,3";
        const Result r = worker.execute(args);
        check(!r.success, "invalid number: fail");
    }

    // Ошибка: разные размерности
    {
        Arguments args;
        args.values["operation"] = "dot";
        args.values["a"] = "1,2";
        args.values["b"] = "1,2,3";
        const Result r = worker.execute(args);
        check(!r.success, "dimension mismatch: fail");
    }

    // Ошибка: normalize нулевого вектора
    {
        Arguments args;
        args.values["operation"] = "normalize";
        args.values["a"] = "0,0";
        const Result r = worker.execute(args);
        check(!r.success, "zero vector: fail");
    }

    std::cout << "\nTotal: " << g_passed << " passed, "
              << g_failed << " failed\n";

    return g_failed == 0 ? 0 : 1;
}
