# ==============================================================================
# CMakeLists.txt - Log Analyzer CLI
# ==============================================================================
# Explanations of core CMake directives:
#
# 1. cmake_minimum_required:
#    Enforces the minimum CMake version to ensure all syntax, policies, and
#    features used in this file are supported by the user's CMake installation.
#
# 2. project():
#    Declares the project name, version, and primary programming languages.
#    This sets up variables like PROJECT_NAME and CMAKE_CURRENT_SOURCE_DIR.
#
# 3. CMAKE_CXX_STANDARD (20):
#    Specifies that C++20 features (such as modern <optional>, spaceship
#    operator, string_view, improved STL) should be enabled across all targets.
#
# 4. add_library():
#    Compiles source files into a reusable library (static by default).
#    Separates the domain logic (parsing, analysis, data models) from the CLI,
#    allowing test executables to link directly against the same code without
#    duplication.
#
# 5. target_include_directories():
#    Specifies header search paths (-I flags). Using PUBLIC means any target
#    that links to 'log_analyzer_lib' automatically inherits access to 'include/'.
#
# 6. add_executable():
#    Instructs CMake to generate a binary target from the specified source file(s).
#
# 7. target_link_libraries():
#    Links an executable or library against another library, resolving symbols
#    and propagating transitive include directories and compile flags.
#
# 8. enable_testing() & add_test():
#    Enables CMake's built-in testing engine (CTest) and registers test binaries
#    so they can be executed together via 'ctest'.
#
# 9. Header/Source Separation:
#    Headers (.hpp) declare interfaces and contracts for callers, while source
#    files (.cpp) contain compiled implementation details. This minimizes build
#    dependencies, reduces rebuild times, and enforces clear encapsulation.
# ==============================================================================

cmake_minimum_required(VERSION 3.20)

project(LogAnalyzer
    VERSION 1.0.0
    DESCRIPTION "A modular C++20 CLI tool for analyzing structured application logs"
    LANGUAGES CXX
)

# Enforce C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable strict compiler warnings to ensure high code quality and prevent bugs
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ------------------------------------------------------------------------------
# Core Library Target
# ------------------------------------------------------------------------------
add_library(log_analyzer_lib
    src/LogEntry.cpp
    src/LogParser.cpp
    src/LogAnalyzer.cpp
)

# Export include directory to consumers of log_analyzer_lib
target_include_directories(log_analyzer_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ------------------------------------------------------------------------------
# Main Application Executable Target
# ------------------------------------------------------------------------------
add_executable(log-analyzer
    src/main.cpp
)

target_link_libraries(log-analyzer
    PRIVATE
        log_analyzer_lib
)

# ------------------------------------------------------------------------------
# Testing Suite with CTest Integration
# ------------------------------------------------------------------------------
enable_testing()

# Parser Unit Tests
add_executable(log_parser_tests
    tests/LogParserTests.cpp
)
target_link_libraries(log_parser_tests
    PRIVATE
        log_analyzer_lib
)
add_test(NAME ParserTests COMMAND log_parser_tests)

# Analyzer Unit Tests
add_executable(log_analyzer_tests
    tests/LogAnalyzerTests.cpp
)
target_link_libraries(log_analyzer_tests
    PRIVATE
        log_analyzer_lib
)
add_test(NAME AnalyzerTests COMMAND log_analyzer_tests)
