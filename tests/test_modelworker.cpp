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

    // ---------- Контракт Worker: name / description / schema ----------
    {
        check(worker.name() == "model", "name() == model");
        check(!worker.description().empty(), "description() non-empty");

        const Schema s = worker.schema();
        check(s.arguments.size() == 3, "schema: 3 arguments");
        check(s.arguments[0].name == "operation", "schema[0] is operation");
        check(s.arguments[1].name == "a", "schema[1] is a");
        check(s.arguments[2].name == "b", "schema[2] is b");
    }

    // ---------- normalize ----------

    // normalize([3,4]) = [0.6, 0.8]
    {
        Arguments args;
        args.values["operation"] = "normalize";
        args.values["a"] = "3,4";
        const Result r = worker.execute(args);
        check(r.success, "normalize [3,4]: success");
        check(r.value.find("0.6") != std::string::npos, "normalize [3,4]: contains 0.6");
        check(r.value.find("0.8") != std::string::npos, "normalize [3,4]: contains 0.8");
    }

    // normalize([1,0]) = [1,0]
    {
        Arguments args;
        args.values["operation"] = "normalize";
        args.values["a"] = "1,0";
        const Result r = worker.execute(args);
        check(r.success, "normalize [1,0]: success");
        check(r.value.find("1") != std::string::npos, "normalize [1,0]: contains 1");
    }

    // normalize([0,5]) = [0,1]
    {
        Arguments args;
        args.values["operation"] = "normalize";
        args.values["a"] = "0,5";
        const Result r = worker.execute(args);
        check(r.success, "normalize [0,5]: success");
        check(r.value.find("1") != std::string::npos, "normalize [0,5]: contains 1");
    }

    // normalize([-3,-4]) = [-0.6,-0.8]
    {
        Arguments args;
        args.values["operation"] = "normalize";
        args.values["a"] = "-3,-4";
        const Result r = worker.execute(args);
        check(r.success, "normalize negative: success");
        check(r.value.find("-0.6") != std::string::npos, "normalize negative: contains -0.6");
        check(r.value.find("-0.8") != std::string::npos, "normalize negative: contains -0.8");
    }

    // normalize([5]) = [1]
    {
        Arguments args;
        args.values["operation"] = "normalize";
        args.values["a"] = "5";
        const Result r = worker.execute(args);
        check(r.success, "normalize single: success");
        check(r.value.find("1") != std::string::npos, "normalize single: contains 1");
    }

    // normalize([0,0]) -> fail
    {
        Arguments args;
        args.values["operation"] = "normalize";
        args.values["a"] = "0,0";
        const Result r = worker.execute(args);
        check(!r.success, "normalize zero: fail");
    }

    // ---------- dot ----------

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

    // ---------- cosine ----------

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

    // ---------- ошибки ----------

    // Нет operation
    {
        Arguments args;
        args.values["a"] = "1,2";
        const Result r = worker.execute(args);
        check(!r.success, "missing operation: fail");
    }

    // Невалидное число
    {
        Arguments args;
        args.values["operation"] = "normalize";
        args.values["a"] = "1,abc,3";
        const Result r = worker.execute(args);
        check(!r.success, "invalid number: fail");
    }

    // Разные размерности
    {
        Arguments args;
        args.values["operation"] = "dot";
        args.values["a"] = "1,2";
        args.values["b"] = "1,2,3";
        const Result r = worker.execute(args);
        check(!r.success, "dimension mismatch: fail");
    }

    // Нет a
    {
        Arguments args;
        args.values["operation"] = "normalize";
        const Result r = worker.execute(args);
        check(!r.success, "missing a: fail");
    }

    // Нет b для dot
    {
        Arguments args;
        args.values["operation"] = "dot";
        args.values["a"] = "1,2";
        const Result r = worker.execute(args);
        check(!r.success, "missing b: fail");
    }

    // Неизвестная операция
    {
        Arguments args;
        args.values["operation"] = "banana";
        args.values["a"] = "1,2";
        const Result r = worker.execute(args);
        check(!r.success, "unknown operation: fail");
    }

    std::cout << "\nTotal: " << g_passed << " passed, "
              << g_failed << " failed\n";

    return g_failed == 0 ? 0 : 1;
}
