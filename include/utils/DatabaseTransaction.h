#pragma once
#include <QSqlDatabase>
#include <QSqlError>
#include "utils/Logger.h"

// RAII wrapper for database transactions.
// Automatically rolls back if commit() is not called before destruction.
class DatabaseTransaction {
public:
    explicit DatabaseTransaction(QSqlDatabase db) : m_db(db), m_committed(false) {
        if (!m_db.transaction()) {
            Logger::error("Failed to start transaction: " + m_db.lastError().text());
        }
    }

    ~DatabaseTransaction() {
        if (!m_committed && m_db.isOpen()) {
            Logger::warning("Transaction rolled back automatically.");
            m_db.rollback();
        }
    }

    bool commit() {
        if (m_db.commit()) {
            m_committed = true;
            return true;
        }
        Logger::error("Failed to commit transaction: " + m_db.lastError().text());
        return false;
    }

private:
    QSqlDatabase m_db;
    bool m_committed;
};