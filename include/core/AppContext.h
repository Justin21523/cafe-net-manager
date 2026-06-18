#pragma once

#include <QObject>

class DatabaseManager;
class SeatRepository;
class SeatService;
class SeatSessionRepository;
class SeatSessionService;
class MenuRepository;
class MenuService;
class OrderRepository;
class OrderService;
class CustomerRepository;
class InventoryService;
class AuditService;

class AppContext : public QObject {
    Q_OBJECT

public:
    explicit AppContext(QObject *parent = nullptr);
    ~AppContext();

    DatabaseManager* databaseManager() const;
    SeatService* seatService() const;
    SeatSessionService* seatSessionService() const;
    MenuService* menuService() const;
    OrderService* orderService() const;
    CustomerRepository* customerRepository() const;
    SeatRepository* seatRepository() const;
    InventoryService* inventoryService() const;
    AuditService* auditService() const;

private:
    DatabaseManager *m_databaseManager;
    SeatRepository *m_seatRepository;
    SeatService *m_seatService;
    SeatSessionRepository *m_sessionRepository;
    SeatSessionService *m_sessionService;
    MenuRepository *m_menuRepository;
    MenuService *m_menuService;
    OrderRepository *m_orderRepository;
    OrderService *m_orderService;
    CustomerRepository *m_customerRepository;
    InventoryService *m_inventoryService;
    AuditService *m_auditService;
};