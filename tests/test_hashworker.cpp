#include "sader/Arguments.h"
#include "sader/HashWorker.h"
#include "sader/Result.h"

#include <iostream>
#include <string>

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
    HashWorker worker;

    {
        Arguments args;
        args.values["operation"] = "md5";
        args.values["text"] = "hello";

        const Result r = worker.execute(args);
        check(r.success, "md5: success");
        check(r.value == "5d41402abc4b2a76b9719d911017c592",
              "md5: value == known reference");
    }

    {
        Arguments args;
        args.values["operation"] = "sha256";
        args.values["text"] = "hello";

        const Result r = worker.execute(args);
        check(r.success, "sha256: success");
        check(r.value == "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824",
              "sha256: value == known reference");
    }

    {
        Arguments args;
        args.values["operation"] = "sha256";
        args.values["text"] = "";

        const Result r = worker.execute(args);
        check(r.success, "sha256 empty: success");
        check(r.value == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
              "sha256 empty: value == known reference");
    }

    {
        Arguments args;
        args.values["text"] = "hello";

        const Result r = worker.execute(args);
        check(!r.success, "missing operation: fail");
    }

    {
        Arguments args;
        args.values["operation"] = "sha256";

        const Result r = worker.execute(args);
        check(!r.success, "missing text: fail");
    }

    {
        Arguments args;
        args.values["operation"] = "banana";
        args.values["text"] = "hello";

        const Result r = worker.execute(args);
        check(!r.success, "unknown algorithm: fail");
    }

    std::cout << "\nTotal: " << g_passed << " passed, "
              << g_failed << " failed\n";

    return g_failed == 0 ? 0 : 1;
}
