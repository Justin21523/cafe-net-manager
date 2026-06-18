#pragma once

#include <QWidget>
#include "models/Seat.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QTimer;
class SeatSessionService;
class OrderService;

class SeatOperationPanel : public QWidget {
    Q_OBJECT

public:
    explicit SeatOperationPanel(SeatSessionService *sessionSvc, OrderService *orderSvc, QWidget *parent = nullptr);

    void loadSeatData(const Seat &seat);
    void clearData();

signals:
    void startSessionRequested(int seatId);
    void endSessionRequested(int seatId);
    void checkoutRequested(int seatId);
    void addOrderRequested(int seatId);

private slots:
    void updateElapsedTime();
    void handleActionClick();

private:
    void setupUI();
    void refreshOrders();

    SeatSessionService *m_sessionSvc;
    OrderService *m_orderSvc;
    
    Seat m_currentSeat;
    int m_currentSessionId = -1;
    
    // UI Elements
    QLabel *m_seatCodeLabel;
    QLabel *m_statusBadge;
    QLabel *m_elapsedTimeLabel;
    QTableWidget *m_ordersTable;
    
    QPushButton *m_startBtn;
    QPushButton *m_addOrderBtn;
    QPushButton *m_checkoutBtn;
    QPushButton *m_endBtn;
    
    QTimer *m_timer;
};