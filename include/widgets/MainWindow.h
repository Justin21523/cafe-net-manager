#pragma once

#include "services/SeatService.h"
#include "models/Seat.h"
#include "models/Order.h"
#include <QMainWindow>
#include <vector>

class SeatMapView;
class SeatDetailPanel;
class QTabWidget;
class MenuBrowserWidget;
class SeatService;
class SeatSessionService;
class MenuService;
class MenuItem; // Forward declaration
class CartWidget;
class OrderService;
class KitchenBoardWidget;
class AdminPage;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initializeSeatMap(const std::vector<Seat> &seats);
    void setServices(SeatService *seatService, SeatSessionService *sessionService, 
                     MenuService *menuService, OrderService *orderService); // 確認簽名

    void setSelectedSeat(int seatId, int sessionId);
    void showAdminPage();

private slots:
    void handleStartSession(int seatId);
    void handleEndSession(int seatId);
    void refreshSeatMap();
    void handleItemAddedToCart(const MenuItem &item);
    void handleCheckout(const Order &order);

private:
    void setupUI();
    
    SeatMapView *m_seatMapView;
    SeatDetailPanel *m_seatDetailPanel;
    QTabWidget *m_rightPanelTabs;
    MenuBrowserWidget *m_menuBrowserWidget;

    SeatService *m_seatService = nullptr;
    SeatSessionService *m_sessionService = nullptr;
    CartWidget *m_cartWidget;
    OrderService *m_orderService = nullptr;
    int m_selectedSeatId = -1;
    int m_selectedSessionId = -1;
    KitchenBoardWidget *m_kitchenBoardWidget;
    AdminPage *m_adminPage;
};