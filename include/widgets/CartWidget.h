#pragma once

#include <QWidget>
#include "services/OrderService.h"

class QTableWidget;
class QLabel;
class QPushButton;

class CartWidget : public QWidget {
    Q_OBJECT

public:
    explicit CartWidget(OrderService *service, QWidget *parent = nullptr);

    void refreshCart();

signals:
    void orderSubmitted();

private slots:
    void handleQuantityChanged(int row);
    void handleRemoveItem();
    void handleSubmitOrder();

private:
    void setupUI();

    OrderService *m_service;
    QTableWidget *m_cartTable;
    QLabel *m_totalLabel;
    QPushButton *m_submitBtn;
    int m_currentSeatId = -1;
    int m_currentSessionId = -1;
};