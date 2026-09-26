#include "sader/ModelWorker.h"

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

// Парсит строку "1.0,2.0,3.0" в вектор double.
// Бросает std::invalid_argument с понятным сообщением при невалидном числе.
std::vector<double> parseVector(const std::string& s) {
    std::vector<double> result;
    std::istringstream stream(s);
    std::string token;

    while (std::getline(stream, token, ',')) {
        if (token.empty()) {
            throw std::invalid_argument("empty number in vector");
        }

        std::size_t pos = 0;
        double value = 0.0;
        try {
            value = std::stod(token, &pos);
        } catch (const std::exception&) {
            throw std::invalid_argument("not a number: '" + token + "'");
        }

        if (pos != token.size()) {
            throw std::invalid_argument("trailing characters in '" + token + "'");
        }

        result.push_back(value);
    }

    if (result.empty()) {
        throw std::invalid_argument("empty vector");
    }
    return result;
}

// Собирает вектор обратно в строку "1.0,2.0,3.0"
std::string formatVector(const std::vector<double>& v) {
    std::ostringstream out;
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i > 0) out << ',';
        out << v[i];
    }
    return out.str();
}

double dotProduct(const std::vector<double>& a, const std::vector<double>& b) {
    double sum = 0.0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

double norm(const std::vector<double>& v) {
    return std::sqrt(dotProduct(v, v));
}

}  // namespace

std::string_view ModelWorker::name() const noexcept {
    return "model";
}

std::string_view ModelWorker::description() const noexcept {
    return "Vector operations: normalize, dot product, cosine similarity";
}

Schema ModelWorker::schema() const {
    return Schema{
        {
            {"operation", "string", true, "Operation: normalize | dot | cosine"},
            {"a",         "string", true, "First vector, comma-separated (e.g. 1,2,3)"},
            {"b",         "string", false, "Second vector (for dot/cosine)"},
        }
    };
}

Result ModelWorker::execute(const Arguments& args) const {
    const auto op_it = args.values.find("operation");
    if (op_it == args.values.end()) {
        return Result::fail("Missing required argument: operation");
    }
    const std::string& operation = op_it->second;

    const auto a_it = args.values.find("a");
    if (a_it == args.values.end()) {
        return Result::fail("Missing required argument: a");
    }

    // Парсим первый вектор — с обработкой ошибок.
    std::vector<double> vec_a;
    try {
        vec_a = parseVector(a_it->second);
    } catch (const std::exception& e) {
        return Result::fail(std::string("Invalid vector a: ") + e.what());
    }

    // normalize — только один вектор нужен.
    if (operation == "normalize") {
        const double n = norm(vec_a);
        if (n == 0.0) {
            return Result::fail("Cannot normalize zero vector");
        }
        std::vector<double> result(vec_a.size());
        for (std::size_t i = 0; i < vec_a.size(); ++i) {
            result[i] = vec_a[i] / n;
        }
        return Result::ok(formatVector(result));
    }

    // dot и cosine — нужны два вектора.
    if (operation == "dot" || operation == "cosine") {
        const auto b_it = args.values.find("b");
        if (b_it == args.values.end()) {
            return Result::fail("Missing required argument: b (needed for " + operation + ")");
        }

        std::vector<double> vec_b;
        try {
            vec_b = parseVector(b_it->second);
        } catch (const std::exception& e) {
            return Result::fail(std::string("Invalid vector b: ") + e.what());
        }

        if (vec_a.size() != vec_b.size()) {
            return Result::fail(
                "Vector dimensions mismatch: a has " + std::to_string(vec_a.size()) +
                ", b has " + std::to_string(vec_b.size())
            );
        }

        if (operation == "dot") {
            return Result::ok(std::to_string(dotProduct(vec_a, vec_b)));
        }

        // cosine
        const double na = norm(vec_a);
        const double nb = norm(vec_b);
        if (na == 0.0 || nb == 0.0) {
            return Result::fail("Cannot compute cosine with zero vector");
        }
        const double cos = dotProduct(vec_a, vec_b) / (na * nb);
        return Result::ok(std::to_string(cos));
    }

    return Result::fail("Unknown operation: " + operation);
}
