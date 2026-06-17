#include "widgets/OrderCardWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QTimer>
#include <QDateTime>
#include <QFrame>

OrderCardWidget::OrderCardWidget(const Order &order, QWidget *parent)
    : QWidget(parent), m_order(order) {
    setupUI();
    
    // Timer to update wait time every second
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &OrderCardWidget::updateWaitTime);
    m_timer->start(1000); // 1000ms = 1s
    
    updateWaitTime(); // Initial update
}

void OrderCardWidget::setupUI(){
    setStyleSheet("background-color: #fff; border: 1px solid #ccc; border-radius: 5px; padding: 10px;");

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Header: Order Number & Seat
    QHBoxLayout *headerLayout = new QHBoxLayout();
    m_orderNumLabel = new QLabel(m_order.orderNumber, this);
    QFont boldFont = m_orderNumLabel->font();
    boldFont.setBold(true);
    m_orderNumLabel->setFont(boldFont);
    headerLayout->addWidget(m_orderNumLabel);
    
    m_seatLabel = new QLabel(QString("Seat: %1").arg(m_order.seatId), this);
    headerLayout->addStretch();
    headerLayout->addWidget(m_seatLabel);
    layout->addLayout(headerLayout);

    // Wait Time
    m_waitTimeLabel = new QLabel("Waiting: 0m", this);
    m_waitTimeLabel->setStyleSheet("color: red; font-weight: bold;");
    layout->addWidget(m_waitTimeLabel);

    // Status Combo Box
    m_statusCombo = new QComboBox(this);
    m_statusCombo->addItem("Submitted", QVariant::fromValue(OrderStatus::Submitted));
    m_statusCombo->addItem("Accepted", QVariant::fromValue(OrderStatus::Accepted));
    m_statusCombo->addItem("Preparing", QVariant::fromValue(OrderStatus::Preparing));
    m_statusCombo->addItem("Ready", QVariant::fromValue(OrderStatus::Ready));
    m_statusCombo->addItem("Served", QVariant::fromValue(OrderStatus::Served));
    
    // Set current status
    int currentIndex = 0;
    switch (m_order.status) {
        case OrderStatus::Submitted: currentIndex = 0; break;
        case OrderStatus::Accepted: currentIndex = 1; break;
        case OrderStatus::Preparing: currentIndex = 2; break;
        case OrderStatus::Ready: currentIndex = 3; break;
        case OrderStatus::Served: currentIndex = 4; break;
        default: break;
    }
    m_statusCombo->setCurrentIndex(currentIndex);

    connect(m_statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        OrderStatus newStatus = static_cast<OrderStatus>(m_statusCombo->itemData(index).toInt());
        emit statusChanged(m_order.id, newStatus);
    });

    layout->addWidget(m_statusCombo);
}

void OrderCardWidget::updateWaitTime() {
    m_waitTimeLabel->setText("Waiting: " + calculateWaitTime());
}

QString OrderCardWidget::calculateWaitTime() const {
    qint64 seconds = m_order.createdAt.secsTo(QDateTime::currentDateTime());
    qint64 minutes = seconds / 60;
    qint64 secs = seconds % 60;
    return QString("%1m %2s").arg(minutes).arg(secs, 2, 10, QChar('0'));
}

void OrderCardWidget::handleStatusChange(const QString &statusText) {
    // Handled by signal in setupUI
}