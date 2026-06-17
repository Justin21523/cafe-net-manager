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

OrderCardWidget::~OrderCardWidget() {
    if (m_timer) {
        m_timer->stop();
        m_timer->deleteLater();
    }
}

void OrderCardWidget::setupUI(){
    setStyleSheet("background-color: #fff; border: 2px solid #ccc; border-radius: 8px; padding: 10px;");
    setMinimumWidth(200);
    setMaximumWidth(300);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(8);

    // Header: Order Number
    m_orderNumLabel = new QLabel(m_order.orderNumber, this);
    QFont boldFont = m_orderNumLabel->font();
    boldFont.setPointSize(12);
    boldFont.setBold(true);
    m_orderNumLabel->setFont(boldFont);
    layout->addWidget(m_orderNumLabel);

    // Seat Info
    m_seatLabel = new QLabel(QString("Seat ID: %1").arg(m_order.seatId), this);
    layout->addWidget(m_seatLabel);

    // Horizontal line
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line);

    // Wait Time
    m_waitTimeLabel = new QLabel("Waiting: 0m 0s", this);
    m_waitTimeLabel->setStyleSheet("color: red; font-weight: bold; font-size: 14px;");
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

    connect(m_statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, [this](int index) {
        OrderStatus newStatus = static_cast<OrderStatus>(m_statusCombo->itemData(index).toInt());
        emit statusChanged(m_order.id, newStatus);
    });

    layout->addWidget(m_statusCombo);
    layout->addStretch();
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