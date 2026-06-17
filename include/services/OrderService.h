#pragma once

#include "models/MenuItem.h"
#include "models/Order.h"
#include <vector>
#include <map>

class OrderRepository;

// CartItem represents an item in the shopping cart (temporary, not yet saved to DB)
struct CartItem {
    MenuItem menuItem;
    int quantity = 1;
};

class OrderService {
public:
    explicit OrderService(OrderRepository *repository);

    // Cart operations
    void addToCart(const MenuItem &item);
    void removeFromCart(int menuItemId);
    void updateQuantity(int menuItemId, int quantity);
    void clearCart();
    std::vector<CartItem> getCartItems() const;
    int calculateCartTotal() const;

    // Order operations
    bool submitOrder(int seatId, int sessionId);
    bool updateOrderStatus(int orderId, OrderStatus status);
    std::vector<Order> getActiveOrders();

private:
    OrderRepository *m_repository;
    std::map<int, CartItem> m_cart; // key: menuItemId
};