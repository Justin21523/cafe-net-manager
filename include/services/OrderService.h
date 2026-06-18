#pragma once

#include "models/MenuItem.h"
#include "models/Order.h"
#include <vector>
#include <map>

class OrderRepository;
class InventoryService;
class AuditService;

// CartItem represents an item in the shopping cart (temporary, not yet saved to DB)
struct CartItem {
    MenuItem menuItem;
    int quantity = 1;
    QString note; // NEW: For modifiers or special requests
};

class OrderService {
public:
    explicit OrderService(OrderRepository *repository);

    // Cart operations
    void addToCart(const MenuItem &item);
    void addToCart(const MenuItem &item, const QString &note);
    void removeFromCart(int menuItemId);
    void updateQuantity(int menuItemId, int quantity);
    void clearCart();
    std::vector<CartItem> getCartItems() const;
    int calculateCartTotal() const;

    // Order operations
    bool submitOrder(int seatId, int sessionId);
    bool updateOrderStatus(int orderId, OrderStatus status);
    std::vector<Order> getActiveOrders();

    std::vector<Order> getOrdersBySeat(int seatId);
    std::vector<OrderItem> getOrderItems(int orderId);
    
    void setInventoryService(InventoryService *invSvc);
    void setAuditService(AuditService *auditSvc);

private:
    OrderRepository *m_repository;
    std::map<int, CartItem> m_cart; // key: menuItemId
    InventoryService *m_inventoryService = nullptr;
    AuditService *m_auditService = nullptr;
};
