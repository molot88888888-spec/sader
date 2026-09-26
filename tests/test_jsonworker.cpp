#include "sader/Arguments.h"
#include "sader/JsonWorker.h"
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
    JsonWorker worker;

    const std::string json =
        R"({"user":{"name":"Alice","age":30},"active":true})";

    // parse — валидный
    {
        Arguments args;
        args.values["operation"] = "parse";
        args.values["json"] = json;
        const Result r = worker.execute(args);
        check(r.success, "parse valid: success");
        check(r.value == "valid", "parse valid: == valid");
    }

    // parse — невалидный
    {
        Arguments args;
        args.values["operation"] = "parse";
        args.values["json"] = "{not json}";
        const Result r = worker.execute(args);
        check(!r.success, "parse invalid: fail");
    }

    // get user.name -> "Alice"
    {
        Arguments args;
        args.values["operation"] = "get";
        args.values["json"] = json;
        args.values["path"] = "user.name";
        const Result r = worker.execute(args);
        check(r.success, "get user.name: success");
        check(r.value == "Alice", "get user.name: == Alice");
    }

    // get user.age -> "30"
    {
        Arguments args;
        args.values["operation"] = "get";
        args.values["json"] = json;
        args.values["path"] = "user.age";
        const Result r = worker.execute(args);
        check(r.success, "get user.age: success");
        check(r.value == "30", "get user.age: == 30");
    }

    // get active -> "true"
    {
        Arguments args;
        args.values["operation"] = "get";
        args.values["json"] = json;
        args.values["path"] = "active";
        const Result r = worker.execute(args);
        check(r.success, "get active: success");
        check(r.value == "true", "get active: == true");
    }

    // type user -> "object"
    {
        Arguments args;
        args.values["operation"] = "type";
        args.values["json"] = json;
        args.values["path"] = "user";
        const Result r = worker.execute(args);
        check(r.success, "type user: success");
        check(r.value == "object", "type user: == object");
    }

    // type user.name -> "string"
    {
        Arguments args;
        args.values["operation"] = "type";
        args.values["json"] = json;
        args.values["path"] = "user.name";
        const Result r = worker.execute(args);
        check(r.success, "type user.name: success");
        check(r.value == "string", "type user.name: == string");
    }

    // Ошибка: путь не существует
    {
        Arguments args;
        args.values["operation"] = "get";
        args.values["json"] = json;
        args.values["path"] = "user.missing";
        const Result r = worker.execute(args);
        check(!r.success, "get missing path: fail");
    }

    // Ошибка: нет json
    {
        Arguments args;
        args.values["operation"] = "parse";
        const Result r = worker.execute(args);
        check(!r.success, "missing json: fail");
    }

    std::cout << "\nTotal: " << g_passed << " passed, "
              << g_failed << " failed\n";

    return g_failed == 0 ? 0 : 1;
}
