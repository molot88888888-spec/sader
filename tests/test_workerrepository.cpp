#include "sader/WorkerRepository.h"

#include <exception>
#include <iostream>
#include <string>

static int g_passed = 0;
static int g_failed = 0;

static void check(bool condition, const std::string& name) {
    if (condition) { std::cout << "PASS: " << name << '\n'; ++g_passed; }
    else { std::cout << "FAIL: " << name << '\n'; ++g_failed; }
}

int main() {
    const std::string conn_str =
        "host=localhost "
        "user=sader "
        "password=.w@CGWlVld6[\"bk& "
        "dbname=sader";

    try {
        WorkerRepository repo(conn_str);

        const auto all = repo.findAllEnabled();
        check(all.size() == 8, "findAllEnabled returns 8 workers");

        bool has_hash = false, has_file = false, has_json = false;
        bool has_http = false, has_text = false, has_csv = false;
        bool has_process = false, has_model = false;

        for (const auto& rec : all) {
            if (rec.name == "hash")    has_hash = true;
            if (rec.name == "file")    has_file = true;
            if (rec.name == "json")    has_json = true;
            if (rec.name == "http")    has_http = true;
            if (rec.name == "text")    has_text = true;
            if (rec.name == "csv")     has_csv = true;
            if (rec.name == "process") has_process = true;
            if (rec.name == "model")   has_model = true;
        }

        check(has_hash,    "contains hash");
        check(has_file,    "contains file");
        check(has_json,    "contains json");
        check(has_http,    "contains http");
        check(has_text,    "contains text");
        check(has_csv,     "contains csv");
        check(has_process, "contains process");
        check(has_model,   "contains model");
    } catch (const std::exception& e) {
        std::cout << "FAIL: exception: " << e.what() << '\n';
        ++g_failed;
    }

    std::cout << "\nTotal: " << g_passed << " passed, "
              << g_failed << " failed\n";

    return g_failed == 0 ? 0 : 1;
}
