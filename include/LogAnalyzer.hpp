#pragma once

#include "LogEntry.hpp"
#include <vector>
#include <string>
#include <map>
#include <iostream>

namespace loganalyzer {

/**
 * @brief Performs statistical aggregation, filtering, and search operations
 * over a collection of parsed LogEntry objects.
 */
class LogAnalyzer {
private:
    std::vector<LogEntry> m_entries;

public:
    /**
     * @brief Constructs an analyzer with a vector of parsed log entries.
     * @param entries Log entries to analyze.
     */
    explicit LogAnalyzer(std::vector<LogEntry> entries);

    /**
     * @brief Returns a const reference to the underlying log entries.
     */
    [[nodiscard]] const std::vector<LogEntry>& getEntries() const noexcept;

    /**
     * @brief Returns the total number of entries stored.
     */
    [[nodiscard]] size_t getTotalEntries() const noexcept;

    /**
     * @brief Counts the number of log entries matching the specified severity level.
     * @param level Severity level to count.
     */
    [[nodiscard]] size_t countBySeverity(LogLevel level) const noexcept;

    /**
     * @brief Generates a breakdown map of count per LogLevel.
     */
    [[nodiscard]] std::map<LogLevel, size_t> getSeverityCounts() const;

    /**
     * @brief Filters log entries matching the specified severity level.
     * @param level Severity level to filter by.
     * @return std::vector<LogEntry> containing only the matching entries.
     */
    [[nodiscard]] std::vector<LogEntry> filterBySeverity(LogLevel level) const;

    /**
     * @brief Searches for log entries whose message contains the given keyword substring.
     * @param keyword Substring to search for within log messages.
     * @return std::vector<LogEntry> containing only entries whose message contains the keyword.
     */
    [[nodiscard]] std::vector<LogEntry> search(const std::string& keyword) const;

    /**
     * @brief Prints the structured summary report to the specified stream.
     * Displays total entries count and breakdown by INFO, WARNING, ERROR, DEBUG.
     * @param out Destination stream (defaults to std::cout).
     */
    void printSummary(std::ostream& out = std::cout) const;
};

} // namespace loganalyzer
