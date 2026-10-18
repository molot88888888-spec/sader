#include "sader/RedisCache.h"

#include <hiredis/hiredis.h>

#include <stdexcept>
#include <utility>

RedisCache::RedisCache(const std::string& host, int port) {
    // redisConnect возвращает context даже при ошибке —
    // проверяем context->err.
    context_ = redisConnect(host.c_str(), port);

    if (context_ == nullptr) {
        throw std::runtime_error("RedisCache: redisConnect returned null (out of memory?)");
    }

    if (context_->err) {
        const std::string err = context_->errstr;
        redisFree(context_);
        context_ = nullptr;
        throw std::runtime_error("RedisCache: cannot connect to Redis: " + err);
    }
}

RedisCache::RedisCache(RedisCache&& other) noexcept
    : context_(other.context_) {
    other.context_ = nullptr;
}

RedisCache& RedisCache::operator=(RedisCache&& other) noexcept {
    if (this != &other) {
        if (context_ != nullptr) {
            redisFree(context_);
        }
        context_ = other.context_;
        other.context_ = nullptr;
    }
    return *this;
}

RedisCache::~RedisCache() {
    if (context_ != nullptr) {
        redisFree(context_);
    }
}

std::optional<std::string> RedisCache::get(const std::string& key) {
    if (context_ == nullptr) {
        return std::nullopt;
    }

    // redisCommand — variadic, формат как у printf:
    // "GET %s" + key.c_str()
    auto* reply = static_cast<redisReply*>(
        redisCommand(context_, "GET %s", key.c_str())
    );

    if (reply == nullptr) {
        // Соединение, скорее всего, потеряно
        return std::nullopt;
    }

    std::optional<std::string> result;

    if (reply->type == REDIS_REPLY_STRING) {
        result = std::string(reply->str, reply->len);
    }
    // REDIS_REPLY_NIL — ключа нет, вернём nullopt

    freeReplyObject(reply);
    return result;
}

bool RedisCache::set(const std::string& key, const std::string& value, int ttl_seconds) {
    if (context_ == nullptr) {
        return false;
    }

    // SET key value EX ttl — атомарно ставит значение с TTL
    auto* reply = static_cast<redisReply*>(
        redisCommand(context_, "SET %s %s EX %d",
                     key.c_str(), value.c_str(), ttl_seconds)
    );

    if (reply == nullptr) {
        return false;
    }

    const bool ok = (reply->type == REDIS_REPLY_STATUS) &&
                    (std::string(reply->str) == "OK");

    freeReplyObject(reply);
    return ok;
}

bool RedisCache::del(const std::string& key) {
    if (context_ == nullptr) {
        return false;
    }

    auto* reply = static_cast<redisReply*>(
        redisCommand(context_, "DEL %s", key.c_str())
    );

    if (reply == nullptr) {
        return false;
    }

    const bool removed = (reply->type == REDIS_REPLY_INTEGER) && (reply->integer > 0);

    freeReplyObject(reply);
    return removed;
}
