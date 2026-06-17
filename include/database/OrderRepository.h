#pragma once

#include "models/Order.h"
#include <vector>

class DatabaseManager;

class OrderRepository {
public:
    explicit OrderRepository(DatabaseManager *dbManager);

    int createOrder(const Order &order);
    bool addOrderItems(int orderId, const std::vector<OrderItem> &items);
    std::vector<Order> getOrdersBySeatId(int seatId);

private:
    DatabaseManager *m_dbManager;
    QString generateOrderNumber();
};