#pragma once

#include <QString>

struct OrderItem {
    int id = -1;
    int orderId = -1;
    int menuItemId = -1;
    QString itemName;
    int quantity = 1;
    int unitPrice = 0; // cents
    int subtotal = 0;  // cents
    QString note;
};