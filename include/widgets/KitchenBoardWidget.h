#pragma once

#include "models/Order.h"
#include <QWidget>

class QVBoxLayout;
class OrderService;

class KitchenBoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit KitchenBoardWidget(OrderService *service, QWidget *parent = nullptr);

    void refreshBoard();

private slots:
    void handleOrderStatusChanged(int orderId, OrderStatus newStatus);

private:
    void setupUI();
    void clearColumns();

    OrderService *m_service;
    QVBoxLayout *m_pendingLayout;
    QVBoxLayout *m_preparingLayout;
    QVBoxLayout *m_readyLayout;
};