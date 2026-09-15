#include "LogAnalyzer.hpp"
#include <algorithm>
#include <iomanip>
#include <utility>

namespace loganalyzer {

LogAnalyzer::LogAnalyzer(std::vector<LogEntry> entries)
    : m_entries(std::move(entries)) {}

const std::vector<LogEntry> &LogAnalyzer::getEntries() const noexcept {
  return m_entries;
}

size_t LogAnalyzer::getTotalEntries() const noexcept {
  return m_entries.size();
}

size_t LogAnalyzer::countBySeverity(LogLevel level) const noexcept {
  return static_cast<size_t>(std::count_if(
      m_entries.begin(), m_entries.end(),
      [level](const LogEntry &entry) { return entry.getLevel() == level; }));
}

std::map<LogLevel, size_t> LogAnalyzer::getSeverityCounts() const {
  std::map<LogLevel, size_t> counts = {{LogLevel::INFO, 0},
                                       {LogLevel::WARNING, 0},
                                       {LogLevel::ERROR, 0},
                                       {LogLevel::DEBUG, 0}};

  for (const auto &entry : m_entries) {
    counts[entry.getLevel()]++;
  }

  return counts;
}

std::vector<LogEntry> LogAnalyzer::filterBySeverity(LogLevel level) const {
  std::vector<LogEntry> results;
  for (const auto &entry : m_entries) {
    if (entry.getLevel() == level) {
      results.push_back(entry);
    }
  }
  return results;
}

std::vector<LogEntry> LogAnalyzer::search(const std::string &keyword) const {
  std::vector<LogEntry> results;
  for (const auto &entry : m_entries) {
    if (entry.getMessage().find(keyword) != std::string::npos) {
      results.push_back(entry);
    }
  }
  return results;
}

void LogAnalyzer::printSummary(std::ostream &out) const {
  out << "Total entries: " << getTotalEntries() << "\n\n";
  out << "INFO:    " << countBySeverity(LogLevel::INFO) << "\n";
  out << "WARNING: " << countBySeverity(LogLevel::WARNING) << "\n";
  out << "ERROR:   " << countBySeverity(LogLevel::ERROR) << "\n";
  out << "DEBUG:   " << countBySeverity(LogLevel::DEBUG) << "\n";
}

} // namespace loganalyzer
