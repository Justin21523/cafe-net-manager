#pragma once

#include "models/Payment.h"

class DatabaseManager;

class PaymentRepository {
public:
    explicit PaymentRepository(DatabaseManager *dbManager);

    bool createPayment(const Payment &payment);
    bool updateOrderStatus(int orderId, const QString &status);

private:
    DatabaseManager *m_dbManager;
};