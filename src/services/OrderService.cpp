#include "services/OrderService.h"
#include "database/OrderRepository.h"
#include "services/InventoryService.h"
#include "services/AuditService.h"
#include "utils/Logger.h"

#include <QDateTime>

OrderService::OrderService(OrderRepository *repository)
    : m_repository(repository) {}

void OrderService::addToCart(const MenuItem &item) {
    addToCart(item, QString());
}

void OrderService::addToCart(const MenuItem &item, const QString &note) {
    if (m_cart.find(item.id) != m_cart.end()) {
        m_cart[item.id].quantity++;
        if (!note.isEmpty()) {
            m_cart[item.id].note = note;
        }
    } else {
        CartItem cartItem;
        cartItem.menuItem = item;
        cartItem.quantity = 1;
        cartItem.note = note;
        m_cart[item.id] = cartItem;
    }
    Logger::info("Added to cart: " + item.name);
}

void OrderService::removeFromCart(int menuItemId) {
    m_cart.erase(menuItemId);
}

void OrderService::updateQuantity(int menuItemId, int quantity) {
    if (quantity <= 0) {
        removeFromCart(menuItemId);
        return;
    }
    if (m_cart.find(menuItemId) != m_cart.end()) {
        m_cart[menuItemId].quantity = quantity;
    }
}

void OrderService::clearCart() {
    m_cart.clear();
}

std::vector<CartItem> OrderService::getCartItems() const {
    std::vector<CartItem> items;
    for (const auto &pair : m_cart) {
        items.push_back(pair.second);
    }
    return items;
}

int OrderService::calculateCartTotal() const {
    int total = 0;
    for (const auto &pair : m_cart) {
        total += pair.second.menuItem.price * pair.second.quantity;
    }
    return total;
}

bool OrderService::submitOrder(int seatId, int sessionId) {
    if (m_cart.empty()) {
        Logger::warning("Cart is empty. Cannot submit order.");
        return false;
    }

    Order order;
    order.seatId = seatId;
    order.sessionId = sessionId;
    order.orderNumber = "ORD-" + QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");
    order.subtotal = calculateCartTotal();
    order.total = order.subtotal;
    order.createdAt = QDateTime::currentDateTime();
    order.status = OrderStatus::Submitted;

    // Convert cart items to order items
    std::vector<OrderItem> orderItems;
    for (const auto &pair : m_cart) {
        OrderItem item;
        item.menuItemId = pair.second.menuItem.id;
        item.itemName = pair.second.menuItem.name;
        item.quantity = pair.second.quantity;
        item.unitPrice = pair.second.menuItem.price;
        item.subtotal = item.unitPrice * item.quantity;
        item.note = pair.second.note; // NEW: Pass note to OrderItem
        orderItems.push_back(item);
    }
    order.items = orderItems;

    int orderId = m_repository->createOrder(order);
    if (orderId == -1) return false;
    
    order.id = orderId; // Update ID for inventory deduction
    m_repository->addOrderItems(orderId, orderItems);

    // --- Phase 3 Integration ---
    // 1. Deduct Inventory
    if (m_inventoryService) {
        if (!m_inventoryService->deductStockForOrder(order)) {
            Logger::warning("Order " + order.orderNumber + " submitted, but inventory deduction failed/rolled back.");
            // 注意：這裡為了不中斷點餐流程，我們不 return false，而是記錄警告。
            // 在真實系統中，可能會要求前端顯示「庫存不足」並取消訂單。
        }
    }

    // 2. Audit Log (Assume Employee ID 1 for demo)
    if (m_auditService) {
        m_auditService->logAction(1, "SUBMIT_ORDER", "orders", orderId, "Seat: " + QString::number(seatId));
    }

    clearCart();
    return true;
}

bool OrderService::updateOrderStatus(int orderId, OrderStatus status) {
    return m_repository->updateOrderStatus(orderId, status);
}

std::vector<Order> OrderService::getActiveOrders() {
    return m_repository->getAllActiveOrders();
}

std::vector<Order> OrderService::getOrdersBySeat(int seatId) {
    return m_repository->getOrdersBySeatId(seatId);
}

std::vector<OrderItem> OrderService::getOrderItems(int orderId) {
    return m_repository->getOrderItems(orderId);
}

void OrderService::setInventoryService(InventoryService *invSvc) { m_inventoryService = invSvc; }
void OrderService::setAuditService(AuditService *auditSvc) { m_auditService = auditSvc; }

