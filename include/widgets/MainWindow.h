#pragma once

#include <QMainWindow>
#include <vector>
#include "models/MenuItem.h"
#include "models/Seat.h"

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

    void setDatabaseManager(DatabaseManager *dbManager);
    void setServices(SeatService *seatService, SeatSessionService *sessionService, 
                     MenuService *menuService, OrderService *orderService,
                     SeatRepository *seatRepository);
                     
    // Call this after setDatabaseManager and setServices
    void initPages(); 
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
    
    SidebarWidget *m_sidebar;
    QStackedWidget *m_stackedWidget;
    
    DashboardPage *m_dashboardPage = nullptr;
    FloorPlanPage *m_floorPlanPage = nullptr;
    PosOrderPage *m_posOrderPage = nullptr;
    KitchenPage *m_kitchenPage = nullptr;

    DatabaseManager *m_dbManager = nullptr;
    SeatService *m_seatService = nullptr;
    SeatSessionService *m_sessionService = nullptr;
    MenuService *m_menuService = nullptr;
    OrderService *m_orderService = nullptr;
    SeatRepository *m_seatRepository = nullptr;
    
    int m_selectedSeatId = -1;
};