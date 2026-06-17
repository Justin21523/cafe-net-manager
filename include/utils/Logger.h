#pragma once // Include guard: prevents multiple inclusions of this header

#include <QString>

// A simple utility class for logging.
// It uses static methods so we don't need to instantiate it.
class Logger {
public:
    static void info(const QString &message);
    static void warning(const QString &message);
    static void error(const QString &message);
};