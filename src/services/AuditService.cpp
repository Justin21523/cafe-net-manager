// AuditService.cpp
#include "services/AuditService.h"
#include "database/DatabaseManager.h"
#include "utils/Logger.h"
#include <QSqlError>
#include <QSqlQuery>

AuditService::AuditService(DatabaseManager *dbManager) : m_dbManager(dbManager) {}

void AuditService::logAction(int employeeId, const QString &action, const QString &table, int targetId, const QString &details) {
    QSqlQuery query(m_dbManager->database());
    query.prepare("INSERT INTO audit_logs (employee_id, action_type, target_table, target_id, details) "
                  "VALUES (:emp_id, :action, :table, :target_id, :details)");
    query.bindValue(":emp_id", employeeId);
    query.bindValue(":action", action);
    query.bindValue(":table", table);
    query.bindValue(":target_id", targetId);
    query.bindValue(":details", details);
    
    if (!query.exec()) {
        Logger::error("Failed to write audit log: " + query.lastError().text());
    }
}