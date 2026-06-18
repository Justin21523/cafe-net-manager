#include "widgets/SeatOperationPanel.h"
#include "services/SeatSessionService.h"
#include "services/OrderService.h"
#include "models/Order.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <QDateTime>
#include <QFrame>

SeatOperationPanel::SeatOperationPanel(SeatSessionService *sessionSvc, OrderService *orderSvc, QWidget *parent)
    : QWidget(parent), m_sessionSvc(sessionSvc), m_orderSvc(orderSvc) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SeatOperationPanel::updateElapsedTime);
    
    setupUI();
}

void SeatOperationPanel::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(15);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    m_seatCodeLabel = new QLabel("Seat --", this);
    QFont titleFont = m_seatCodeLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    m_seatCodeLabel->setFont(titleFont);
    headerLayout->addWidget(m_seatCodeLabel);
    
    m_statusBadge = new QLabel("Available", this);
    m_statusBadge->setStyleSheet("background-color: #4CAF50; color: white; padding: 4px 8px; border-radius: 4px; font-weight: bold;");
    headerLayout->addStretch();
    headerLayout->addWidget(m_statusBadge);
    layout->addLayout(headerLayout);

    // Session Info Box
    QFrame *sessionBox = new QFrame(this);
    sessionBox->setStyleSheet("background-color: #f5f5f5; border-radius: 8px; padding: 10px;");
    QVBoxLayout *sessionLayout = new QVBoxLayout(sessionBox);
    
    m_elapsedTimeLabel = new QLabel("Elapsed: --", this);
    m_elapsedTimeLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #333;");
    sessionLayout->addWidget(m_elapsedTimeLabel);
    layout->addWidget(sessionBox);

    // Orders Table
    QLabel *ordersTitle = new QLabel("Recent Orders", this);
    ordersTitle->setStyleSheet("font-weight: bold; font-size: 14px;");
    layout->addWidget(ordersTitle);

    m_ordersTable = new QTableWidget(this);
    m_ordersTable->setColumnCount(3);
    m_ordersTable->setHorizontalHeaderLabels({"Order #", "Status", "Total"});
    m_ordersTable->horizontalHeader()->setStretchLastSection(true);
    m_ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ordersTable->verticalHeader()->setVisible(false);
    layout->addWidget(m_ordersTable, 1); // Stretch

    // Action Buttons
    m_startBtn = new QPushButton("Start Session", this);
    m_startBtn->setStyleSheet("background-color: #4CAF50; color: white; padding: 10px; font-weight: bold; border-radius: 4px;");
    connect(m_startBtn, &QPushButton::clicked, this, [this]() { emit startSessionRequested(m_currentSeat.id); });
    layout->addWidget(m_startBtn);

    m_addOrderBtn = new QPushButton("Add Order (POS)", this);
    m_addOrderBtn->setStyleSheet("background-color: #2196F3; color: white; padding: 10px; font-weight: bold; border-radius: 4px;");
    connect(m_addOrderBtn, &QPushButton::clicked, this, [this]() { emit addOrderRequested(m_currentSeat.id); });
    layout->addWidget(m_addOrderBtn);

    m_checkoutBtn = new QPushButton("Checkout", this);
    m_checkoutBtn->setStyleSheet("background-color: #FF9800; color: white; padding: 10px; font-weight: bold; border-radius: 4px;");
    connect(m_checkoutBtn, &QPushButton::clicked, this, [this]() { emit checkoutRequested(m_currentSeat.id); });
    layout->addWidget(m_checkoutBtn);

    m_endBtn = new QPushButton("End Session", this);
    m_endBtn->setStyleSheet("background-color: #f44336; color: white; padding: 10px; font-weight: bold; border-radius: 4px;");
    connect(m_endBtn, &QPushButton::clicked, this, [this]() { emit endSessionRequested(m_currentSeat.id); });
    layout->addWidget(m_endBtn);

    clearData();
}

void SeatOperationPanel::loadSeatData(const Seat &seat) {
    m_currentSeat = seat;
    m_seatCodeLabel->setText("Seat " + seat.code);
    
    // Update Status Badge
    QString statusStr = SeatStatusHelper::toString(seat.status);
    m_statusBadge->setText(statusStr);
    if (seat.status == SeatStatus::Available) {
        m_statusBadge->setStyleSheet("background-color: #4CAF50; color: white; padding: 4px 8px; border-radius: 4px; font-weight: bold;");
    } else if (seat.status == SeatStatus::Occupied) {
        m_statusBadge->setStyleSheet("background-color: #f44336; color: white; padding: 4px 8px; border-radius: 4px; font-weight: bold;");
    } else {
        m_statusBadge->setStyleSheet("background-color: #9E9E9E; color: white; padding: 4px 8px; border-radius: 4px; font-weight: bold;");
    }

    // Handle Session & Timer
    m_timer->stop();
    m_currentSessionId = -1;
    
    if (seat.status == SeatStatus::Occupied && m_sessionSvc) {
        auto sessionOpt = m_sessionSvc->getActiveSession(seat.id);
        if (sessionOpt.has_value()) {
            m_currentSessionId = sessionOpt->id;
            m_timer->start(1000); // Start ticking
            updateElapsedTime();
        }
    } else {
        m_elapsedTimeLabel->setText("Elapsed: --");
    }

    // Handle Orders
    refreshOrders();

    // Toggle Buttons based on status
    bool isAvailable = (seat.status == SeatStatus::Available);
    bool isOccupied = (seat.status == SeatStatus::Occupied);

    m_startBtn->setVisible(isAvailable);
    m_addOrderBtn->setVisible(isOccupied);
    m_checkoutBtn->setVisible(isOccupied);
    m_endBtn->setVisible(isOccupied);
}

void SeatOperationPanel::clearData() {
    m_currentSeat = Seat();
    m_currentSessionId = -1;
    if (m_timer) {
        m_timer->stop();
    }
    
    m_seatCodeLabel->setText("Select a Seat");
    m_statusBadge->setText("--");
    m_elapsedTimeLabel->setText("Elapsed: --");
    m_ordersTable->setRowCount(0);
    
    m_startBtn->setVisible(false);
    m_addOrderBtn->setVisible(false);
    m_checkoutBtn->setVisible(false);
    m_endBtn->setVisible(false);
}

void SeatOperationPanel::updateElapsedTime() {
    if (m_currentSessionId == -1 || !m_sessionSvc) return;
    
    auto sessionOpt = m_sessionSvc->getActiveSession(m_currentSeat.id);
    if (sessionOpt.has_value()) {
        qint64 secs = sessionOpt->startTime.secsTo(QDateTime::currentDateTime());
        int hours = secs / 3600;
        int mins = (secs % 3600) / 60;
        int seconds = secs % 60;
        
        m_elapsedTimeLabel->setText(QString("Elapsed: %1h %2m %3s")
            .arg(hours, 2, 10, QChar('0'))
            .arg(mins, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0')));
    }
}

void SeatOperationPanel::handleActionClick() {
    // Button actions are connected directly in setupUI().
}

void SeatOperationPanel::refreshOrders() {
    m_ordersTable->setRowCount(0);
    if (!m_orderSvc) return;

    std::vector<Order> orders = m_orderSvc->getOrdersBySeat(m_currentSeat.id);
    
    // Filter only active/unpaid orders for display
    for (const auto &order : orders) {
        if (order.status == OrderStatus::Paid || order.status == OrderStatus::Cancelled) continue;

        int row = m_ordersTable->rowCount();
        m_ordersTable->insertRow(row);
        
        m_ordersTable->setItem(row, 0, new QTableWidgetItem(order.orderNumber));
        
        QString statusStr = "Unknown";
        switch (order.status) {
            case OrderStatus::Submitted: statusStr = "Submitted"; break;
            case OrderStatus::Preparing: statusStr = "Preparing"; break;
            case OrderStatus::Ready: statusStr = "Ready"; break;
            case OrderStatus::Served: statusStr = "Served"; break;
            default: break;
        }
        m_ordersTable->setItem(row, 1, new QTableWidgetItem(statusStr));
        m_ordersTable->setItem(row, 2, new QTableWidgetItem(QString("$%1").arg(order.total / 100.0, 0, 'f', 2)));
    }
}
