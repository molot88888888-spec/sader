#pragma once

#include <map>
#include <string>
#include <variant>

// DiscoverCommand — результат разбора "DISCOVER <query>".
struct DiscoverCommand {
    std::string query;
};

// DescribeCommand — результат разбора "DESCRIBE <name>".
struct DescribeCommand {
    std::string worker_name;
};

// CallCommand — результат разбора "CALL <worker> <operation> <args...>".
// Например: CALL text length --text=hello
//   worker_name = "text"
//   operation   = "length"
//   args        = { "text": "hello" }
struct CallCommand {
    std::string worker_name;
    std::string operation;
    std::map<std::string, std::string> args;
};

// Command — это ОДНА из трёх команд.
// std::variant гарантирует: в любой момент там ровно один вариант,
// не ноль и не больше одного.
using Command = std::variant<DiscoverCommand, DescribeCommand, CallCommand>;
