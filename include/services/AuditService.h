#pragma once
#include <QString>
class DatabaseManager;

class AuditService {
public:
    explicit AuditService(DatabaseManager *dbManager);
    void logAction(int employeeId, const QString &action, const QString &table, int targetId, const QString &details = "");
private:
    DatabaseManager *m_dbManager;
};