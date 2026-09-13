#include "LogParser.hpp"
#include "LogAnalyzer.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace {

void printHelp(const std::string& programName) {
    std::cout << "Log Analyzer CLI - High-performance C++20 structured log analyzer\n\n"
              << "Usage:\n"
              << "  " << programName << " <filepath> summary\n"
              << "  " << programName << " <filepath> filter <LEVEL>\n"
              << "  " << programName << " <filepath> search <keyword>\n"
              << "  " << programName << " --help\n\n"
              << "Commands:\n"
              << "  summary          Display total log count and breakdown by severity.\n"
              << "  filter <LEVEL>   Output all log lines matching the specified level\n"
              << "                   (Valid levels: INFO, WARNING, ERROR, DEBUG).\n"
              << "  search <keyword> Output all log lines containing the keyword.\n"
              << "  --help, -h       Display this help message and exit.\n\n"
              << "Examples:\n"
              << "  " << programName << " logs/app.log summary\n"
              << "  " << programName << " logs/app.log filter ERROR\n"
              << "  " << programName << " logs/app.log search database\n";
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: No arguments provided.\n\n";
        printHelp("log-analyzer");
        return 1;
    }

    const std::string firstArg = argv[1];
    if (firstArg == "--help" || firstArg == "-h") {
        printHelp(argv[0]);
        return 0;
    }

    if (argc < 3) {
        std::cerr << "Error: Missing command. Expected 'summary', 'filter', or 'search'.\n\n";
        printHelp(argv[0]);
        return 1;
    }

    const std::string filepath = argv[1];
    const std::string command = argv[2];

    try {
        loganalyzer::LogParser parser;
        size_t malformedCount = 0;
        std::vector<loganalyzer::LogEntry> entries = parser.parseFile(filepath, &malformedCount);
        loganalyzer::LogAnalyzer analyzer(std::move(entries));

        if (command == "summary") {
            analyzer.printSummary();
            return 0;
        }

        if (command == "filter") {
            if (argc < 4) {
                std::cerr << "Error: 'filter' command requires a severity level argument (INFO, WARNING, ERROR, DEBUG).\n";
                return 1;
            }

            const std::string levelStr = argv[3];
            auto levelOpt = loganalyzer::stringToLogLevel(levelStr);
            if (!levelOpt.has_value()) {
                std::cerr << "Error: Unknown log level '" << levelStr << "'.\n"
                          << "Valid levels are: INFO, WARNING, ERROR, DEBUG.\n";
                return 1;
            }

            const auto results = analyzer.filterBySeverity(*levelOpt);
            for (const auto& entry : results) {
                std::cout << entry.toString() << "\n";
            }
            return 0;
        }

        if (command == "search") {
            if (argc < 4) {
                std::cerr << "Error: 'search' command requires a keyword argument.\n";
                return 1;
            }

            const std::string keyword = argv[3];
            const auto results = analyzer.search(keyword);
            for (const auto& entry : results) {
                std::cout << entry.toString() << "\n";
            }
            return 0;
        }

        std::cerr << "Error: Unknown command '" << command << "'.\n\n";
        printHelp(argv[0]);
        return 1;

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
