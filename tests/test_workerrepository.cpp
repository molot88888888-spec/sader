#include "sader/RedisCache.h"
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
        RedisCache cache("127.0.0.1", 6379);

        // Очищаем кэш перед началом — чтобы тест был детерминированным
        cache.del("sader:workers:all");

        WorkerRepository repo(conn_str, cache);

        // 1. Первый вызов — из PostgreSQL, кэш пуст → должен заполниться
        {
            const auto all = repo.findAllEnabled();
            check(all.size() == 8, "first call: 8 workers from PostgreSQL");

            const auto cached = cache.get("sader:workers:all");
            check(cached.has_value(), "cache populated after first call");
        }

        // 2. Второй вызов — из кэша (тот же результат)
        {
            const auto all = repo.findAllEnabled();
            check(all.size() == 8, "second call: 8 workers from cache");
        }

        // 3. Проверяем, что при изменении БД кэш возвращает старые данные
        {
            pqxx::connection conn(conn_str);
            pqxx::work txn(conn);
            txn.exec(
                "UPDATE workers SET description = 'CACHED_OLD' WHERE name = 'hash'"
            );
            txn.commit();
        }

        // Третий вызов — должен вернуть СТАРОЕ описание (из кэша)
        {
            const auto all = repo.findAllEnabled();
            bool found_old = false;
            for (const auto& r : all) {
                if (r.name == "hash" && r.description == "CACHED_OLD") {
                    // Данные из PG (не из кэша) — тест провалится
                }
                if (r.name == "hash" &&
                    r.description == "Calculate md5 and sha256 hashes for text") {
                    found_old = true;
                }
            }
            check(found_old, "third call: old description from cache (not updated)");
        }

        // 4. Очищаем кэш — теперь должен вернуться CACHED_OLD из PostgreSQL
        cache.del("sader:workers:all");

        {
            const auto all = repo.findAllEnabled();
            bool found_new = false;
            for (const auto& r : all) {
                if (r.name == "hash" && r.description == "CACHED_OLD") {
                    found_new = true;
                }
            }
            check(found_new, "after cache clear: new description from PostgreSQL");
        }

        // 5. Возвращаем БД в исходное состояние
        {
            pqxx::connection conn(conn_str);
            pqxx::work txn(conn);
            txn.exec(
                "UPDATE workers SET description = "
                "'Calculate md5 and sha256 hashes for text' WHERE name = 'hash'"
            );
            txn.commit();
        }

        // 6. Очищаем кэш на всякий случай
        cache.del("sader:workers:all");

    } catch (const std::exception& e) {
        std::cout << "FAIL: exception: " << e.what() << '\n';
        ++g_failed;
    }

    std::cout << "\nTotal: " << g_passed << " passed, "
              << g_failed << " failed\n";

    return g_failed == 0 ? 0 : 1;
}

