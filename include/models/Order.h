#pragma once

#include <QString>
#include <QDateTime>
#include <vector>
#include "models/OrderItem.h"

enum class OrderStatus {
    Draft,
    Submitted,
    Accepted,
    Preparing,
    Ready,
    Served,
    Paid,
    Cancelled
};

struct Order {
    int id = -1;
    int sessionId = -1;
    int seatId = -1;
    QString orderNumber;
    OrderStatus status = OrderStatus::Draft;
    int subtotal = 0;
    int discount = 0;
    int serviceCharge = 0;
    int total = 0;
    QDateTime createdAt;
    std::vector<OrderItem> items;
};