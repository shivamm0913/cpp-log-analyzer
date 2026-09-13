#pragma once

#include "LogEntry.hpp"
#include <vector>
#include <string>
#include <optional>
#include <iostream>

namespace loganalyzer {

/**
 * @brief Responsible for parsing raw log strings and files into LogEntry objects.
 * Encapsulates validation and robust error recovery for malformed lines.
 */
class LogParser {
public:
    LogParser() = default;

    /**
     * @brief Parses a single line of log text into a LogEntry.
     * Expected format: YYYY-MM-DD HH:MM:SS LEVEL MESSAGE
     *
     * @param line The raw text line to parse.
     * @return std::optional containing the LogEntry if valid, or std::nullopt if malformed.
     */
    [[nodiscard]] std::optional<LogEntry> parseLine(const std::string& line) const;

    /**
     * @brief Reads a log file and parses all valid log entries.
     * Malformed lines are reported via warning and skipped without terminating the process.
     *
     * @param filepath Path to the log file on disk.
     * @param outMalformedCount Optional pointer to receive the count of skipped malformed lines.
     * @param warnStream Optional stream for malformed line warning messages (defaults to std::cerr).
     * @return std::vector<LogEntry> containing all successfully parsed entries in sequential order.
     * @throws std::runtime_error if the file cannot be opened (fatal error).
     */
    [[nodiscard]] std::vector<LogEntry> parseFile(
        const std::string& filepath,
        size_t* outMalformedCount = nullptr,
        std::ostream* warnStream = &std::cerr
    ) const;

private:
    [[nodiscard]] static bool isValidDate(const std::string& date) noexcept;
    [[nodiscard]] static bool isValidTime(const std::string& time) noexcept;
};

} // namespace loganalyzer
