# Log Analyzer CLI

A modular, interview-ready **C++20 command-line log analysis tool** designed to parse structured application log files, calculate severity statistics, perform keyword search and filtering, and handle malformed input safely without crashing.

---

## Table of Contents

- [Overview](#overview)
- [Architecture & Design](#architecture--design)
- [Features](#features)
- [Project Structure](#project-structure)
- [Building the Project](#building-the-project)
- [Running Unit Tests](#running-unit-tests)
- [Usage & Examples](#usage--examples)
- [Key Engineering Decisions](#key-engineering-decisions)
- [Limitations & Future Improvements](#limitations--future-improvements)

---

## Overview

Application log files can grow to tens of thousands of lines, making manual inspection error-prone and tedious. The **Log Analyzer CLI** parses structured log files conforming to the standard pattern:

```text
YYYY-MM-DD HH:MM:SS LEVEL MESSAGE
```

Supported severity levels are `INFO`, `WARNING`, `ERROR`, and `DEBUG`. The tool extracts structured records into memory, aggregates statistics, filters by severity level, searches messages for substrings, and gracefully recovers from malformed entries.

---

## Architecture & Design

The application enforces strict **Single Responsibility Principle (SRP)**:

```text
               +-----------------------------+
               |         main / CLI          |  (src/main.cpp)
               |  - Parses command arguments |
               |  - Manages stream I/O       |
               +--------------+--------------+
                              |
                              v
               +-----------------------------+
               |          LogParser          |  (include/LogParser.hpp, src/LogParser.cpp)
               |  - Validates date/time      |
               |  - Tokenizes log lines      |
               |  - Skips malformed lines    |
               +--------------+--------------+
                              | yields std::vector<LogEntry>
                              v
               +-----------------------------+
               |         LogAnalyzer         |  (include/LogAnalyzer.hpp, src/LogAnalyzer.cpp)
               |  - Severity counts          |
               |  - Filtering                |
               |  - Substring search         |
               +-----------------------------+
```

### Component Breakdown

1. **`LogEntry`**: Represents a single immutable log record (`timestamp`, `LogLevel`, `message`). Knows nothing about files, arguments, or statistics.
2. **`LogParser`**: Responsible for input format validation and string extraction. Distinguishes fatal errors (missing file) from recoverable errors (malformed individual line).
3. **`LogAnalyzer`**: Operates strictly on already-validated `LogEntry` records in memory. Implements $O(N)$ scanning algorithms for filtering, statistics, and keyword search.
4. **`CLI / main`**: Handles user input, orchestrates parsing and analysis, and formats console output.

---

## Features

- **Strict Validation**: Validates date (`YYYY-MM-DD`), time (`HH:MM:SS`), and known log levels.
- **Fault-Tolerant Parsing**: Skips malformed lines with descriptive warnings while continuing to process valid lines.
- **Severity Aggregation**: Calculates total entry count and breakdown across `INFO`, `WARNING`, `ERROR`, and `DEBUG`.
- **Level Filtering**: Extracts and displays entries matching a specific severity level.
- **Keyword Search**: Performs case-sensitive substring searching within log messages.
- **Zero External Dependencies**: Implemented purely using the C++20 standard library (`<optional>`, `<vector>`, `<map>`, `<string>`, `<algorithm>`, `<fstream>`).
- **Comprehensive Unit Tests**: Built-in test executables for both parser and analyzer, integrated with CTest.

---

## Project Structure

```text
log-analyzer/
├── CMakeLists.txt              # Build configuration and CTest setup
├── README.md                   # Project documentation
├── .gitignore                  # Git ignore rules
│
├── include/
│   ├── LogEntry.hpp            # LogLevel enum and LogEntry class declarations
│   ├── LogParser.hpp           # LogParser declaration
│   └── LogAnalyzer.hpp         # LogAnalyzer declaration
│
├── src/
│   ├── LogEntry.cpp            # LogEntry & LogLevel implementations
│   ├── LogParser.cpp           # Line & file parsing implementation
│   ├── LogAnalyzer.cpp         # Statistical and search operations
│   └── main.cpp                # CLI entry point
│
├── tests/
│   ├── LogParserTests.cpp      # Unit tests for parsing and validation
│   └── LogAnalyzerTests.cpp    # Unit tests for aggregation, search, and filtering
│
├── sample_data/
│   └── sample.log              # Sample log file containing valid and malformed lines
│
└── docs/
    └── INTERVIEW_NOTES.md      # Detailed Q&A and explanations for technical interviews
```

---

## Building the Project

### Prerequisites
- C++20 compliant compiler (GCC 11+, Clang 13+, or MSVC 2019+)
- CMake 3.20+
- Build system (Ninja or GNU Make)

### Build Commands

```bash
# Configure the build directory
cmake -S . -B build -G "Ninja"

# Compile all targets (library, CLI binary, test executables)
cmake --build build
```

---

## Running Unit Tests

Run all tests through CMake's test runner:

```bash
ctest --test-dir build --output-on-failure
```

Or run the individual test executables directly:

```bash
./build/log_parser_tests
./build/log_analyzer_tests
```

---

## Usage & Examples

### 1. View Summary Statistics
```bash
./build/log-analyzer sample_data/sample.log summary
```
**Output:**
```text
Warning: skipped malformed line 6
Warning: skipped malformed line 13
Total entries: 12

INFO:    6
WARNING: 2
ERROR:   2
DEBUG:   2
```

### 2. Filter by Severity Level
```bash
./build/log-analyzer sample_data/sample.log filter ERROR
```
**Output:**
```text
Warning: skipped malformed line 6
Warning: skipped malformed line 13
2026-09-12 10:17:11 ERROR Database connection failed after 3 attempts
2026-09-12 10:17:15 ERROR Database connection failed on backup replica
```

### 3. Search by Keyword
```bash
./build/log-analyzer sample_data/sample.log search Database
```
**Output:**
```text
Warning: skipped malformed line 6
Warning: skipped malformed line 13
2026-09-12 10:16:03 WARNING Database response slow (took 452ms)
2026-09-12 10:17:11 ERROR Database connection failed after 3 attempts
2026-09-12 10:17:15 ERROR Database connection failed on backup replica
```

### 4. Help Message
```bash
./build/log-analyzer --help
```

---

## Key Engineering Decisions

### Why `std::vector<LogEntry>`?
- Logs are inherently sequential records where chronological order matters.
- `std::vector` provides contiguous memory allocation, offering optimal cache locality and minimal memory overhead during sequential iterations.
- We do not need random key-based lookup for the primary log store.

### Why `enum class LogLevel`?
- Scoped enumeration prevents naming collisions in the global namespace.
- Disallows implicit conversions to integers or strings, guaranteeing strong type safety across the application.

### Why Separate Parser and Analyzer?
- Mixing parsing logic with statistical aggregation leads to high coupling and low testability.
- Decoupling allows testing parser edge cases (e.g., malformed date tokens, empty messages) independently from analyzer logic (e.g., aggregation math, keyword matching).

### Complexity Analysis
- **Parsing**: $O(N)$ time, single linear pass over file lines.
- **Summary**: $O(N)$ time, single linear scan over $N$ parsed entries.
- **Filter**: $O(N)$ time, predicate evaluation per entry.
- **Search**: $O(N \cdot M)$ time, where $M$ is average message length.
- **Memory**: $O(N)$, storing parsed entries sequentially in RAM.

---

## Limitations & Future Improvements

1. **Memory Bound**: Large log files (e.g., 50 GB+) cannot fit entirely into RAM. A future version can implement streaming analysis that aggregates statistics in a single pass without storing all entries in memory.
2. **Case Sensitivity**: Substring search is currently case-sensitive. Case-insensitive search or regex matching can be added if required.
3. **Date Filtering**: Range queries on timestamps (e.g., filter between timestamps $T_1$ and $T_2$) can be implemented using `std::chrono`.
