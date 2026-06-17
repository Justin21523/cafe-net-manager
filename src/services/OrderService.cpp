#include "services/OrderService.h"
#include "database/OrderRepository.h"
#include "utils/Logger.h"

#include <QDateTime>

OrderService::OrderService(OrderRepository *repository)
    : m_repository(repository) {}

void OrderService::addToCart(const MenuItem &item) {
    if (m_cart.find(item.id) != m_cart.end()) {
        m_cart[item.id].quantity++;
    } else {
        CartItem cartItem;
        cartItem.menuItem = item;
        cartItem.quantity = 1;
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
    order.total = order.subtotal; // Simplified: no discount/service charge yet
    order.createdAt = QDateTime::currentDateTime();

    // Convert cart items to order items
    std::vector<OrderItem> orderItems;
    for (const auto &pair : m_cart) {
        OrderItem item;
        item.menuItemId = pair.second.menuItem.id;
        item.itemName = pair.second.menuItem.name;
        item.quantity = pair.second.quantity;
        item.unitPrice = pair.second.menuItem.price;
        item.subtotal = item.unitPrice * item.quantity;
        orderItems.push_back(item);
    }

    int orderId = m_repository->createOrder(order);
    if (orderId == -1) return false;

    if (!m_repository->addOrderItems(orderId, orderItems)) {
        return false;
    }

    Logger::info("Order submitted: " + order.orderNumber);
    clearCart();
    return true;
}