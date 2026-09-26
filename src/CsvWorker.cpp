#include "sader/CsvWorker.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

// Разбивает строку по разделителю (для CSV — ',').
std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> cells;
    std::istringstream stream(line);
    std::string cell;
    while (std::getline(stream, cell, ',')) {
        cells.push_back(cell);
    }
    return cells;
}

// Извлекает все значения из указанной колонки.
// Первая строка считается заголовком и пропускается.
// Бросает исключение, если колонки нет или число невалидно.
std::vector<double> extractColumn(const std::string& data, std::size_t column_index) {
    std::istringstream stream(data);
    std::string line;

    std::vector<double> values;
    bool is_header = true;

    while (std::getline(stream, line)) {
        if (line.empty()) continue;

        if (is_header) {
            is_header = false;
            // Проверяем, что в заголовке вообще есть такая колонка
            const auto header = splitCsvLine(line);
            if (column_index >= header.size()) {
                throw std::invalid_argument(
                    "column index " + std::to_string(column_index) +
                    " out of range (header has " + std::to_string(header.size()) + " columns)"
                );
            }
            continue;
        }

        const auto cells = splitCsvLine(line);
        if (column_index >= cells.size()) {
            throw std::invalid_argument(
                "row has fewer columns than index " + std::to_string(column_index)
            );
        }

        const std::string& cell = cells[column_index];
        std::size_t pos = 0;
        double value = 0.0;
        try {
            value = std::stod(cell, &pos);
        } catch (const std::exception&) {
            throw std::invalid_argument("not a number: '" + cell + "'");
        }
        if (pos != cell.size()) {
            throw std::invalid_argument("trailing characters in '" + cell + "'");
        }
        values.push_back(value);
    }

    return values;
}

}  // namespace

std::string_view CsvWorker::name() const noexcept {
    return "csv";
}

std::string_view CsvWorker::description() const noexcept {
    return "CSV operations: count rows, sum/mean/min/max of a numeric column";
}

Schema CsvWorker::schema() const {
    return Schema{
        {
            {"operation", "string", true, "Operation: count | sum | mean | min | max"},
            {"data",      "string", true, "CSV content (newlines as \\n)"},
            {"column",    "string", false, "Column index (0-based, required for sum/mean/min/max)"},
        }
    };
}

Result CsvWorker::execute(const Arguments& args) const {
    const auto op_it = args.values.find("operation");
    if (op_it == args.values.end()) {
        return Result::fail("Missing required argument: operation");
    }
    const std::string& operation = op_it->second;

    const auto data_it = args.values.find("data");
    if (data_it == args.values.end()) {
        return Result::fail("Missing required argument: data");
    }
    const std::string& data = data_it->second;

    // count — просто считаем строки (без заголовка)
    if (operation == "count") {
        std::istringstream stream(data);
        std::string line;
        std::size_t rows = 0;
        bool is_header = true;
        while (std::getline(stream, line)) {
            if (line.empty()) continue;
            if (is_header) { is_header = false; continue; }
            ++rows;
        }
        return Result::ok(std::to_string(rows));
    }

    // Остальные операции — нужна колонка
    const auto col_it = args.values.find("column");
    if (col_it == args.values.end()) {
        return Result::fail("Missing required argument: column (needed for " + operation + ")");
    }

    std::size_t column_index = 0;
    try {
        column_index = static_cast<std::size_t>(std::stoul(col_it->second));
    } catch (const std::exception&) {
        return Result::fail("Invalid column index: '" + col_it->second + "'");
    }

    std::vector<double> values;
    try {
        values = extractColumn(data, column_index);
    } catch (const std::exception& e) {
        return Result::fail(std::string("Cannot extract column: ") + e.what());
    }

    if (values.empty()) {
        return Result::fail("No data rows found");
    }

    if (operation == "sum") {
        double sum = 0.0;
        for (double v : values) sum += v;
        return Result::ok(std::to_string(sum));
    }

    if (operation == "mean") {
        double sum = 0.0;
        for (double v : values) sum += v;
        return Result::ok(std::to_string(sum / values.size()));
    }

    if (operation == "min") {
        return Result::ok(std::to_string(*std::min_element(values.begin(), values.end())));
    }

    if (operation == "max") {
        return Result::ok(std::to_string(*std::max_element(values.begin(), values.end())));
    }

    return Result::fail("Unknown operation: " + operation);
}
