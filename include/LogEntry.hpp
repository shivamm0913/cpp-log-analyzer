#pragma once

#include <string>
#include <optional>
#include <iostream>

namespace loganalyzer {

/**
 * @brief Represents the severity level of a log entry.
 * Using an 'enum class' provides scoped values and strong type safety,
 * avoiding arbitrary string comparisons throughout analysis routines.
 */
enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

/**
 * @brief Converts a LogLevel enum value to its string representation.
 */
std::string logLevelToString(LogLevel level);

/**
 * @brief Converts a string to its corresponding LogLevel enum value.
 * @return std::optional containing the LogLevel if valid, or std::nullopt if unknown.
 */
std::optional<LogLevel> stringToLogLevel(const std::string& levelStr);

/**
 * @brief Represents a single structured log entry.
 * Contains the timestamp, severity level, and log message.
 * Employs standard value semantics and const-correctness.
 */
class LogEntry {
private:
    std::string m_timestamp;
    LogLevel m_level;
    std::string m_message;

public:
    /**
     * @brief Constructs a new LogEntry.
     * @param timestamp Formatted timestamp string (e.g. "YYYY-MM-DD HH:MM:SS").
     * @param level Severity level of the log entry.
     * @param message Text payload of the log entry.
     */
    LogEntry(std::string timestamp, LogLevel level, std::string message);

    /**
     * @brief Gets the timestamp of the log entry.
     */
    [[nodiscard]] const std::string& getTimestamp() const noexcept;

    /**
     * @brief Gets the log level of the entry.
     */
    [[nodiscard]] LogLevel getLevel() const noexcept;

    /**
     * @brief Gets the message content of the log entry.
     */
    [[nodiscard]] const std::string& getMessage() const noexcept;

    /**
     * @brief Formats the log entry back into standard log format:
     * "YYYY-MM-DD HH:MM:SS LEVEL MESSAGE"
     */
    [[nodiscard]] std::string toString() const;

    /**
     * @brief Default equality operator for unit testing and comparisons.
     */
    bool operator==(const LogEntry& other) const = default;
};

} // namespace loganalyzer
