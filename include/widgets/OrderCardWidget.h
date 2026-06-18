#pragma once

#include <QWidget>
#include "models/Order.h"

class QLabel;
class QPushButton;
class QTimer;
class QVBoxLayout;

class OrderCardWidget : public QWidget {
    Q_OBJECT

public:
    explicit OrderCardWidget(const Order &order, QWidget *parent = nullptr);
    ~OrderCardWidget();
    
signals:
    void statusChanged(int orderId, OrderStatus newStatus);

private slots:
    void updateWaitTime();
    void handleStatusChange(const QString &statusText);

private:
    void setupUI();
    void updateActionButton();
    QString calculateWaitTime() const;

    Order m_order;

    // UI Elements
    QLabel *m_headerLabel;
    QLabel *m_waitTimeLabel;
    QVBoxLayout *m_itemsLayout;
    QPushButton *m_actionBtn;
    QTimer *m_timer;
};