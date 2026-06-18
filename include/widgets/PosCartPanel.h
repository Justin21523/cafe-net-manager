#pragma once
#include <QWidget>
#include "services/OrderService.h"

class QLabel;
class QTableWidget;
class QPushButton;

class PosCartPanel : public QWidget {
    Q_OBJECT
public:
    explicit PosCartPanel(OrderService *service, QWidget *parent = nullptr);

    void setTargetSeat(int seatId, const QString &seatCode);
    void refreshCart();

signals:
    void sendToKitchenRequested();
    void checkoutRequested();

public slots:
    void handleAddItem(const MenuItem &item, const QString &note);

private slots:
    void handleQtyChanged(int row, int value);
    void handleRemoveItem(int row);

private:
    void setupUI();
    void updateTotals();

    OrderService *m_service;
    
    QLabel *m_seatInfoLabel;
    QTableWidget *m_cartTable;
    QLabel *m_totalLabel;
    QPushButton *m_sendBtn;
    QPushButton *m_checkoutBtn;
    
    int m_targetSeatId = -1;
};
