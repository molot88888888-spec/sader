#include "sader/CommandParser.h"
#include "sader/StringUtils.h"

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

Command parseCommand(std::string_view line) {
    const std::vector<std::string_view> tokens = split(line, ' ');

    if (tokens.empty()) {
        throw std::invalid_argument("Empty command");
    }

    const std::string_view verb = tokens[0];

    // DISCOVER <query>
    if (verb == "DISCOVER") {
        if (tokens.size() < 2) {
            throw std::invalid_argument("DISCOVER requires a query");
        }
        // Склеиваем все токены после DISCOVER обратно через пробел,
        // чтобы поддержать multi-word query: "DISCOVER sha256 hash"
        std::string query;
        for (std::size_t i = 1; i < tokens.size(); ++i) {
            if (i > 1) query += ' ';
            query += tokens[i];
        }
        return DiscoverCommand{std::move(query)};
    }

    // DESCRIBE <worker>
    if (verb == "DESCRIBE") {
        if (tokens.size() != 2) {
            throw std::invalid_argument("DESCRIBE requires exactly one worker name");
        }
        return DescribeCommand{std::string(tokens[1])};
    }

    // CALL <worker> <operation> [--key=value ...]
    if (verb == "CALL") {
        if (tokens.size() < 3) {
            throw std::invalid_argument("CALL requires: CALL <worker> <operation> [args]");
        }

        CallCommand cmd;
        cmd.worker_name = std::string(tokens[1]);
        cmd.operation   = std::string(tokens[2]);

        for (std::size_t i = 3; i < tokens.size(); ++i) {
            auto [key, value] = parse_key_value(tokens[i]);
            cmd.args[std::string(key)] = std::string(value);
        }

        return cmd;
    }

    throw std::invalid_argument("Unknown or unsupported command: " + std::string(verb));
}
