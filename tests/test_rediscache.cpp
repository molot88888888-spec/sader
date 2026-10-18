#include "sader/RedisCache.h"

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
    try {
        RedisCache cache("127.0.0.1", 6379);

        const std::string test_key = "sader:test:key";

        // Удаляем ключ — на случай, если остался от прошлого запуска
        cache.del(test_key);

        // 1. get на несуществующий ключ → nullopt
        {
            const auto v = cache.get(test_key);
            check(!v.has_value(), "get missing key: nullopt");
        }

        // 2. set + get
        {
            const bool ok = cache.set(test_key, "hello", 60);
            check(ok, "set: returns true");

            const auto v = cache.get(test_key);
            check(v.has_value(), "get after set: has value");
            check(v.value() == "hello", "get after set: == 'hello'");
        }

        // 3. перезапись
        {
            cache.set(test_key, "world", 60);
            const auto v = cache.get(test_key);
            check(v.value() == "world", "overwrite: == 'world'");
        }

        // 4. del
        {
            const bool removed = cache.del(test_key);
            check(removed, "del: returns true");

            const auto v = cache.get(test_key);
            check(!v.has_value(), "get after del: nullopt");
        }

        // 5. del несуществующего ключа
        {
            const bool removed = cache.del(test_key);
            check(!removed, "del missing: returns false");
        }

        // 6. значение с пробелами и спецсимволами
        {
            const std::string tricky = "hello world | $ @ # \"quotes\"";
            cache.set(test_key, tricky, 60);
            const auto v = cache.get(test_key);
            check(v.has_value() && v.value() == tricky,
                  "get tricky value: matches original");
            cache.del(test_key);
        }

    } catch (const std::exception& e) {
        std::cout << "FAIL: exception: " << e.what() << '\n';
        ++g_failed;
    }

    std::cout << "\nTotal: " << g_passed << " passed, "
              << g_failed << " failed\n";

    return g_failed == 0 ? 0 : 1;
}
