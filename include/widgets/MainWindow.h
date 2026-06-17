#pragma once

#include <QMainWindow>
#include <vector>
#include "models/Seat.h"
#include "models/MenuItem.h"

class SidebarWidget;
class QStackedWidget;

class DashboardPage;
class FloorPlanPage;
class PosOrderPage;
class KitchenPage;

class SeatService;
class SeatSessionService;
class MenuService;
class OrderService;
class DatabaseManager;
class SeatRepository;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Dependency Injection
    void setDatabaseManager(DatabaseManager *dbManager);
    void setServices(SeatService *seatService, SeatSessionService *sessionService, 
                     MenuService *menuService, OrderService *orderService,
                     SeatRepository *seatRepository);
                     
    // Initialize Pages (Must be called AFTER setServices)
    void initPages(); 
    
    // Load initial data
    void initializeSeatMap(const std::vector<Seat> &seats);

private slots:
    void handleNavigateTo(int index);
    
    // Floor Plan signals
    void handleStartSession(int seatId);
    void handleEndSession(int seatId);
    void handleExtendSession(int sessionId, int minutes);
    void handleSeatPositionChanged(int seatId, int x, int y);
    
    // POS signals
    void handleItemAddedToCart(const MenuItem &item);
    void handleOrderSubmitted();

private:
    void setupUI();
    void connectSignals();
    
    // UI Components
    SidebarWidget *m_sidebar;
    QStackedWidget *m_stackedWidget;
    
    // Pages
    DashboardPage *m_dashboardPage = nullptr;
    FloorPlanPage *m_floorPlanPage = nullptr;
    PosOrderPage *m_posOrderPage = nullptr;
    KitchenPage *m_kitchenPage = nullptr;

    // Services & DB
    DatabaseManager *m_dbManager = nullptr;
    SeatService *m_seatService = nullptr;
    SeatSessionService *m_sessionService = nullptr;
    MenuService *m_menuService = nullptr;
    OrderService *m_orderService = nullptr;
    SeatRepository *m_seatRepository = nullptr;
    
    int m_selectedSeatId = -1;
};