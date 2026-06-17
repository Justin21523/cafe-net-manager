#pragma once

#include <QWidget>
#include "models/Order.h"

class QLabel;
class QComboBox;
class QTimer;

class OrderCardWidget : public QWidget {
    Q_OBJECT

public:
    explicit OrderCardWidget(const Order &order, QWidget *parent = nullptr);

signals:
    void statusChanged(int orderId, OrderStatus newStatus);

private slots:
    void updateWaitTime();
    void handleStatusChange(const QString &statusText);

private:
    void setupUI();
    QString calculateWaitTime() const;

    Order m_order;
    QLabel *m_orderNumLabel;
    QLabel *m_seatLabel;
    QLabel *m_waitTimeLabel;
    QComboBox *m_statusCombo;
    QTimer *m_timer;
};