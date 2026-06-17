#include "database/OrderRepository.h"
#include "database/DatabaseManager.h"
#include "utils/Logger.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

OrderRepository::OrderRepository(DatabaseManager *dbManager)
    : m_dbManager(dbManager) {}

QString OrderRepository::generateOrderNumber() {
    // Simple order number: ORD-YYYYMMDD-HHMMSS
    return "ORD-" + QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");
}

int OrderRepository::createOrder(const Order &order) {
    QSqlQuery query(m_dbManager->database());
    query.prepare("INSERT INTO orders (session_id, seat_id, order_number, status, subtotal, discount, service_charge, total, created_at) "
                  "VALUES (:session_id, :seat_id, :order_number, :status, :subtotal, :discount, :service_charge, :total, :created_at)");
    
    // 修正：如果 sessionId 無效，則插入 NULL，避免 Foreign Key 錯誤
    if (order.sessionId > 0) {
        query.bindValue(":session_id", order.sessionId);
    } else {
        query.bindValue(":session_id", QVariant()); 
    }

    query.bindValue(":seat_id", order.seatId);
    query.bindValue(":order_number", order.orderNumber);
    query.bindValue(":status", "Submitted"); // 確保狀態是 Submitted
    query.bindValue(":subtotal", order.subtotal);
    query.bindValue(":discount", order.discount);
    query.bindValue(":service_charge", order.serviceCharge);
    query.bindValue(":total", order.total);
    query.bindValue(":created_at", order.createdAt.toString(Qt::ISODate));

    if (!query.exec()) {
        Logger::error("Failed to create order: " + query.lastError().text());
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool OrderRepository::addOrderItems(int orderId, const std::vector<OrderItem> &items) {
    QSqlQuery query(m_dbManager->database());
    query.prepare("INSERT INTO order_items (order_id, menu_item_id, item_name, quantity, unit_price, subtotal, note) "
                  "VALUES (:order_id, :menu_item_id, :item_name, :quantity, :unit_price, :subtotal, :note)");

    for (const auto &item : items) {
        query.bindValue(":order_id", orderId);
        query.bindValue(":menu_item_id", item.menuItemId);
        query.bindValue(":item_name", item.itemName);
        query.bindValue(":quantity", item.quantity);
        query.bindValue(":unit_price", item.unitPrice);
        query.bindValue(":subtotal", item.subtotal);
        query.bindValue(":note", item.note);

        if (!query.exec()) {
            Logger::error("Failed to add order item: " + query.lastError().text());
            return false;
        }
    }
    return true;
}

std::vector<Order> OrderRepository::getOrdersBySeatId(int seatId) {
    std::vector<Order> orders;
    QSqlQuery query(m_dbManager->database());
    query.prepare("SELECT id, session_id, seat_id, order_number, status, subtotal, discount, service_charge, total, created_at "
                  "FROM orders WHERE seat_id = :seat_id ORDER BY created_at DESC");
    query.bindValue(":seat_id", seatId);

    if (!query.exec()) {
        Logger::error("Failed to fetch orders: " + query.lastError().text());
        return orders;
    }

    while (query.next()) {
        Order order;
        order.id = query.value("id").toInt();
        order.sessionId = query.value("session_id").toInt();
        order.seatId = query.value("seat_id").toInt();
        order.orderNumber = query.value("order_number").toString();
        order.status = OrderStatus::Draft; // Simplified
        order.subtotal = query.value("subtotal").toInt();
        order.discount = query.value("discount").toInt();
        order.serviceCharge = query.value("service_charge").toInt();
        order.total = query.value("total").toInt();
        order.createdAt = query.value("created_at").toDateTime();
        orders.push_back(order);
    }
    return orders;
}

std::vector<Order> OrderRepository::getAllActiveOrders() {
    std::vector<Order> orders;
    QSqlQuery query(m_dbManager->database());
    // Fetch orders that are not yet Paid or Cancelled
    query.prepare("SELECT id, session_id, seat_id, order_number, status, subtotal, discount, service_charge, total, created_at "
                  "FROM orders ORDER BY created_at DESC");

    if (!query.exec()) {
        Logger::error("Failed to fetch active orders: " + query.lastError().text());
        return orders;
    }

    while (query.next()) {
        Order order;
        order.id = query.value("id").toInt();
        order.sessionId = query.value("session_id").toInt();
        order.seatId = query.value("seat_id").toInt();
        order.orderNumber = query.value("order_number").toString();
        
        QString statusStr = query.value("status").toString();
        if (statusStr == "Draft") order.status = OrderStatus::Draft;
        else if (statusStr == "Submitted") order.status = OrderStatus::Submitted;
        else if (statusStr == "Accepted") order.status = OrderStatus::Accepted;
        else if (statusStr == "Preparing") order.status = OrderStatus::Preparing;
        else if (statusStr == "Ready") order.status = OrderStatus::Ready;
        else if (statusStr == "Served") order.status = OrderStatus::Served;
        else if (statusStr == "Paid") order.status = OrderStatus::Paid;
        else if (statusStr == "Cancelled") order.status = OrderStatus::Cancelled;

        order.subtotal = query.value("subtotal").toInt();
        order.discount = query.value("discount").toInt();
        order.serviceCharge = query.value("service_charge").toInt();
        order.total = query.value("total").toInt();
        order.createdAt = query.value("created_at").toDateTime();
        orders.push_back(order);
    }
    return orders;
}

bool OrderRepository::updateOrderStatus(int orderId, OrderStatus status) {
    QSqlQuery query(m_dbManager->database());
    query.prepare("UPDATE orders SET status = :status, updated_at = CURRENT_TIMESTAMP WHERE id = :id");
    
    QString statusStr;
    switch (status) {
        case OrderStatus::Draft: statusStr = "Draft"; break;
        case OrderStatus::Submitted: statusStr = "Submitted"; break;
        case OrderStatus::Accepted: statusStr = "Accepted"; break;
        case OrderStatus::Preparing: statusStr = "Preparing"; break;
        case OrderStatus::Ready: statusStr = "Ready"; break;
        case OrderStatus::Served: statusStr = "Served"; break;
        case OrderStatus::Paid: statusStr = "Paid"; break;
        case OrderStatus::Cancelled: statusStr = "Cancelled"; break;
    }

    query.bindValue(":status", statusStr);
    query.bindValue(":id", orderId);

    if (!query.exec()) {
        Logger::error("Failed to update order status: " + query.lastError().text());
        return false;
    }
    return true;
}