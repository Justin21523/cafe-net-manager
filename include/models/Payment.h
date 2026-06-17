#pragma once

#include <QString>
#include <QDateTime>

enum class PaymentMethod {
    Cash,
    CreditCard,
    MemberBalance
};

enum class PaymentStatus {
    Completed,
    Pending,
    Failed
};

struct Payment {
    int id = -1;
    int orderId = -1;
    int amount = 0; // cents
    PaymentMethod method = PaymentMethod::Cash;
    PaymentStatus status = PaymentStatus::Pending;
    int cashReceived = 0; // for cash payment
    int change = 0; // for cash payment
    QDateTime createdAt;
};