#include "utils/Logger.h"
#include <QDebug>

void Logger::info(const QString &message) {
    qDebug().noquote() << "[INFO]" << message;
}

void Logger::warning(const QString &message) {
    qWarning().noquote() << "[WARNING]" << message;
}

void Logger::error(const QString &message) {
    qCritical().noquote() << "[ERROR]" << message;
}