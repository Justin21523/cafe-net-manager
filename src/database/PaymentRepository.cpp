#include "database/PaymentRepository.h"
#include "database/DatabaseManager.h"
#include "utils/Logger.h"

#include <QSqlQuery>
#include <QSqlError>

PaymentRepository::PaymentRepository(DatabaseManager *dbManager)
    : m_dbManager(dbManager) {}

bool PaymentRepository::createPayment(const Payment &payment) {
    QSqlQuery query(m_dbManager->database());
    query.prepare("INSERT INTO payments (order_id, amount, method, status, created_at) "
                  "VALUES (:order_id, :amount, :method, :status, :created_at)");
    query.bindValue(":order_id", payment.orderId);
    query.bindValue(":amount", payment.amount);
    
    QString methodStr;
    switch (payment.method) {
        case PaymentMethod::Cash: methodStr = "Cash"; break;
        case PaymentMethod::CreditCard: methodStr = "CreditCard"; break;
        case PaymentMethod::MemberBalance: methodStr = "MemberBalance"; break;
    }
    query.bindValue(":method", methodStr);
    query.bindValue(":status", "Completed");
    query.bindValue(":created_at", payment.createdAt.toString(Qt::ISODate));

    if (!query.exec()) {
        Logger::error("Failed to create payment: " + query.lastError().text());
        return false;
    }
    return true;
}

bool PaymentRepository::updateOrderStatus(int orderId, const QString &status) {
    QSqlQuery query(m_dbManager->database());
    query.prepare("UPDATE orders SET status = :status, updated_at = CURRENT_TIMESTAMP WHERE id = :id");
    query.bindValue(":status", status);
    query.bindValue(":id", orderId);

    if (!query.exec()) {
        Logger::error("Failed to update order status: " + query.lastError().text());
        return false;
    }
    return true;
}