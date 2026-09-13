#include "LogAnalyzer.hpp"
#include <iostream>
#include <vector>
#include <sstream>

namespace {

int totalTests = 0;
int passedTests = 0;

void recordTest(const std::string& testName, bool success) {
    ++totalTests;
    if (success) {
        ++passedTests;
        std::cout << "  [PASS] " << testName << "\n";
    } else {
        std::cerr << "  [FAIL] " << testName << "\n";
    }
}

std::vector<loganalyzer::LogEntry> createSampleEntries() {
    return {
        {"2026-09-12 10:15:21", loganalyzer::LogLevel::INFO, "Application started"},
        {"2026-09-12 10:15:24", loganalyzer::LogLevel::INFO, "User logged in"},
        {"2026-09-12 10:16:03", loganalyzer::LogLevel::WARNING, "Database response slow"},
        {"2026-09-12 10:17:11", loganalyzer::LogLevel::ERROR, "Database connection failed"},
        {"2026-09-12 10:17:15", loganalyzer::LogLevel::ERROR, "Database timeout retry exceeded"},
        {"2026-09-12 10:18:04", loganalyzer::LogLevel::DEBUG, "Garbage collector cycle complete"}
    };
}

bool testTotalCount() {
    loganalyzer::LogAnalyzer analyzer(createSampleEntries());
    return analyzer.getTotalEntries() == 6;
}

bool testSeverityCounts() {
    loganalyzer::LogAnalyzer analyzer(createSampleEntries());
    return analyzer.countBySeverity(loganalyzer::LogLevel::INFO) == 2 &&
           analyzer.countBySeverity(loganalyzer::LogLevel::WARNING) == 1 &&
           analyzer.countBySeverity(loganalyzer::LogLevel::ERROR) == 2 &&
           analyzer.countBySeverity(loganalyzer::LogLevel::DEBUG) == 1;
}

bool testFilterBySeverity() {
    loganalyzer::LogAnalyzer analyzer(createSampleEntries());
    auto errorEntries = analyzer.filterBySeverity(loganalyzer::LogLevel::ERROR);
    if (errorEntries.size() != 2) return false;
    return errorEntries[0].getMessage() == "Database connection failed" &&
           errorEntries[1].getMessage() == "Database timeout retry exceeded";
}

bool testSearchKeyword() {
    loganalyzer::LogAnalyzer analyzer(createSampleEntries());
    auto dbEntries = analyzer.search("Database");
    if (dbEntries.size() != 3) return false;

    auto nonExistent = analyzer.search("NonExistentKeywordXYZ");
    return nonExistent.empty();
}

bool testEmptyEntries() {
    loganalyzer::LogAnalyzer emptyAnalyzer(std::vector<loganalyzer::LogEntry>{});
    if (emptyAnalyzer.getTotalEntries() != 0) return false;
    if (emptyAnalyzer.countBySeverity(loganalyzer::LogLevel::INFO) != 0) return false;
    if (emptyAnalyzer.countBySeverity(loganalyzer::LogLevel::ERROR) != 0) return false;
    if (!emptyAnalyzer.filterBySeverity(loganalyzer::LogLevel::ERROR).empty()) return false;
    if (!emptyAnalyzer.search("test").empty()) return false;

    std::ostringstream oss;
    emptyAnalyzer.printSummary(oss);
    return oss.str().find("Total entries: 0") != std::string::npos;
}

bool testSummaryOutput() {
    loganalyzer::LogAnalyzer analyzer(createSampleEntries());
    std::ostringstream oss;
    analyzer.printSummary(oss);
    const std::string out = oss.str();
    return out.find("Total entries: 6") != std::string::npos &&
           out.find("INFO:    2") != std::string::npos &&
           out.find("WARNING: 1") != std::string::npos &&
           out.find("ERROR:   2") != std::string::npos &&
           out.find("DEBUG:   1") != std::string::npos;
}

} // namespace

int main() {
    std::cout << "Running LogAnalyzer Tests...\n";

    recordTest("Total entries count", testTotalCount());
    recordTest("Severity breakdown counts", testSeverityCounts());
    recordTest("Filter entries by severity", testFilterBySeverity());
    recordTest("Keyword search matching and non-matching", testSearchKeyword());
    recordTest("Empty entries edge case", testEmptyEntries());
    recordTest("Summary stream output formatting", testSummaryOutput());

    std::cout << "LogAnalyzer Tests: " << passedTests << "/" << totalTests << " passed.\n";
    return (passedTests == totalTests) ? 0 : 1;
}
