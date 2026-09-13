#include "LogParser.hpp"
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdexcept>

namespace loganalyzer {

bool LogParser::isValidDate(const std::string& date) noexcept {
    // Expected format: YYYY-MM-DD (10 characters)
    if (date.length() != 10) {
        return false;
    }
    if (date[4] != '-' || date[7] != '-') {
        return false;
    }
    for (size_t i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) {
            continue;
        }
        if (!std::isdigit(static_cast<unsigned char>(date[i]))) {
            return false;
        }
    }
    return true;
}

bool LogParser::isValidTime(const std::string& time) noexcept {
    // Expected format: HH:MM:SS (8 characters)
    if (time.length() != 8) {
        return false;
    }
    if (time[2] != ':' || time[5] != ':') {
        return false;
    }
    for (size_t i = 0; i < 8; ++i) {
        if (i == 2 || i == 5) {
            continue;
        }
        if (!std::isdigit(static_cast<unsigned char>(time[i]))) {
            return false;
        }
    }
    return true;
}

std::optional<LogEntry> LogParser::parseLine(const std::string& line) const {
    if (line.empty()) {
        return std::nullopt;
    }

    std::string cleanLine = line;
    // Strip trailing carriage return '\r' if file uses Windows CRLF
    if (!cleanLine.empty() && cleanLine.back() == '\r') {
        cleanLine.pop_back();
    }

    std::istringstream iss(cleanLine);
    std::string date;
    std::string time;
    std::string levelStr;

    // Extract the first three space-delimited fields: Date, Time, Level
    if (!(iss >> date >> time >> levelStr)) {
        return std::nullopt;
    }

    if (!isValidDate(date) || !isValidTime(time)) {
        return std::nullopt;
    }

    auto levelOpt = stringToLogLevel(levelStr);
    if (!levelOpt.has_value()) {
        return std::nullopt;
    }

    // Extract the remainder of the line as the message
    std::string message;
    std::getline(iss, message);

    // Strip leading whitespace preceding the message
    const auto firstNonSpace = message.find_first_not_of(" \t");
    if (firstNonSpace == std::string::npos) {
        // Message is missing or only contains whitespace
        return std::nullopt;
    }
    message = message.substr(firstNonSpace);

    return LogEntry(date + " " + time, *levelOpt, std::move(message));
}

std::vector<LogEntry> LogParser::parseFile(
    const std::string& filepath,
    size_t* outMalformedCount,
    std::ostream* warnStream
) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filepath);
    }

    std::vector<LogEntry> entries;
    std::string line;
    size_t lineNumber = 0;
    size_t malformedCount = 0;

    while (std::getline(file, line)) {
        ++lineNumber;

        // Skip completely blank lines
        if (line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }

        auto entryOpt = parseLine(line);
        if (entryOpt.has_value()) {
            entries.push_back(std::move(*entryOpt));
        } else {
            ++malformedCount;
            if (warnStream != nullptr) {
                *warnStream << "Warning: skipped malformed line " << lineNumber << "\n";
            }
        }
    }

    if (outMalformedCount != nullptr) {
        *outMalformedCount = malformedCount;
    }

    return entries;
}

} // namespace loganalyzer
