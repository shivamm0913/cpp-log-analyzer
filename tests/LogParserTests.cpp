#include "LogParser.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

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

bool testValidInfoLine() {
    loganalyzer::LogParser parser;
    auto result = parser.parseLine("2026-09-12 10:15:21 INFO Application started");
    if (!result.has_value()) return false;
    return result->getTimestamp() == "2026-09-12 10:15:21" &&
           result->getLevel() == loganalyzer::LogLevel::INFO &&
           result->getMessage() == "Application started";
}

bool testValidErrorLine() {
    loganalyzer::LogParser parser;
    auto result = parser.parseLine("2026-09-12 10:17:11 ERROR Database connection failed");
    if (!result.has_value()) return false;
    return result->getTimestamp() == "2026-09-12 10:17:11" &&
           result->getLevel() == loganalyzer::LogLevel::ERROR &&
           result->getMessage() == "Database connection failed";
}

bool testValidMessageWithSpaces() {
    loganalyzer::LogParser parser;
    auto result = parser.parseLine("2026-09-12 10:16:03 WARNING Database response slow after 30 seconds timeout");
    if (!result.has_value()) return false;
    return result->getTimestamp() == "2026-09-12 10:16:03" &&
           result->getLevel() == loganalyzer::LogLevel::WARNING &&
           result->getMessage() == "Database response slow after 30 seconds timeout";
}

bool testValidDebugLine() {
    loganalyzer::LogParser parser;
    auto result = parser.parseLine("2026-09-12 10:14:00 DEBUG Cache warmed up with 42 entries");
    if (!result.has_value()) return false;
    return result->getLevel() == loganalyzer::LogLevel::DEBUG &&
           result->getMessage() == "Cache warmed up with 42 entries";
}

bool testUnknownLevel() {
    loganalyzer::LogParser parser;
    auto result = parser.parseLine("2026-09-12 10:17:11 CRITICAL System is burning");
    return !result.has_value();
}

bool testMissingMessage() {
    loganalyzer::LogParser parser;
    auto result1 = parser.parseLine("2026-09-12 10:17:11 ERROR");
    auto result2 = parser.parseLine("2026-09-12 10:17:11 ERROR   ");
    return !result1.has_value() && !result2.has_value();
}

bool testMalformedLines() {
    loganalyzer::LogParser parser;
    bool allMalformed = true;
    allMalformed = allMalformed && !parser.parseLine("this is not a valid log line").has_value();
    allMalformed = allMalformed && !parser.parseLine("").has_value();
    allMalformed = allMalformed && !parser.parseLine("2026-09-12 ERROR Missing time").has_value();
    allMalformed = allMalformed && !parser.parseLine("99-99-9999 10:17:11 ERROR Invalid date").has_value();
    allMalformed = allMalformed && !parser.parseLine("2026-09-12 10:17 ERROR Invalid time").has_value();
    return allMalformed;
}

bool testFileParsingWithMalformedLines() {
    const std::string tempFile = "temp_parser_test.log";
    {
        std::ofstream out(tempFile);
        out << "2026-09-12 10:00:01 INFO First valid\n";
        out << "malformed line here\n";
        out << "2026-09-12 10:00:02 ERROR Second valid\n";
        out << "2026-09-12 10:00:03 UNKNOWN Invalid level\n";
        out << "2026-09-12 10:00:04 WARNING Third valid\n";
    }

    loganalyzer::LogParser parser;
    size_t malformedCount = 0;
    std::ostringstream warnStream;
    auto entries = parser.parseFile(tempFile, &malformedCount, &warnStream);

    std::filesystem::remove(tempFile);

    return entries.size() == 3 &&
           malformedCount == 2 &&
           entries[0].getMessage() == "First valid" &&
           entries[1].getMessage() == "Second valid" &&
           entries[2].getMessage() == "Third valid";
}

} // namespace

int main() {
    std::cout << "Running LogParser Tests...\n";

    recordTest("Valid INFO line", testValidInfoLine());
    recordTest("Valid ERROR line", testValidErrorLine());
    recordTest("Valid message with multiple spaces", testValidMessageWithSpaces());
    recordTest("Valid DEBUG line", testValidDebugLine());
    recordTest("Rejection of unknown level", testUnknownLevel());
    recordTest("Rejection of missing message", testMissingMessage());
    recordTest("Rejection of malformed lines", testMalformedLines());
    recordTest("File parsing with malformed line skipping", testFileParsingWithMalformedLines());

    std::cout << "LogParser Tests: " << passedTests << "/" << totalTests << " passed.\n";
    return (passedTests == totalTests) ? 0 : 1;
}
