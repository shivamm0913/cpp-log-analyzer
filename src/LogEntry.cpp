#include "LogEntry.hpp"
#include <utility>

namespace loganalyzer {

std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::DEBUG:   return "DEBUG";
    }
    return "UNKNOWN";
}

std::optional<LogLevel> stringToLogLevel(const std::string& levelStr) {
    if (levelStr == "INFO")    return LogLevel::INFO;
    if (levelStr == "WARNING") return LogLevel::WARNING;
    if (levelStr == "ERROR")   return LogLevel::ERROR;
    if (levelStr == "DEBUG")   return LogLevel::DEBUG;
    return std::nullopt;
}

LogEntry::LogEntry(std::string timestamp, LogLevel level, std::string message)
    : m_timestamp(std::move(timestamp))
    , m_level(level)
    , m_message(std::move(message)) {}

const std::string& LogEntry::getTimestamp() const noexcept {
    return m_timestamp;
}

LogLevel LogEntry::getLevel() const noexcept {
    return m_level;
}

const std::string& LogEntry::getMessage() const noexcept {
    return m_message;
}

std::string LogEntry::toString() const {
    return m_timestamp + " " + logLevelToString(m_level) + " " + m_message;
}

} // namespace loganalyzer
