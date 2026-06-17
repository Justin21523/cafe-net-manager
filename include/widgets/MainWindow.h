#pragma once

#include "services/SeatService.h"
#include <QMainWindow>
#include <vector>
#include <models/Seat.h>

class SeatMapView;
class SeatDetailPanel;
class QTabWidget;
class MenuBrowserWidget;
class SeatService;
class SeatSessionService;
class MenuService;
class MenuItem; // Forward declaration

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initializeSeatMap(const std::vector<Seat> &seats);
    void setServices(SeatService *seatService, SeatSessionService *sessionService, MenuService *menuService);

private slots:
    void handleStartSession(int seatId);
    void handleEndSession(int seatId);
    void refreshSeatMap();
    void handleItemAddedToCart(const MenuItem &item);

private:
    void setupUI();
    
    SeatMapView *m_seatMapView;
    SeatDetailPanel *m_seatDetailPanel;
    QTabWidget *m_rightPanelTabs;
    MenuBrowserWidget *m_menuBrowserWidget;

    SeatService *m_seatService = nullptr;
    SeatSessionService *m_sessionService = nullptr;
};