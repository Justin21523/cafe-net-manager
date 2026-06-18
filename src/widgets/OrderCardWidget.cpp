#include "widgets/OrderCardWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QDateTime>
#include <QFrame>

OrderCardWidget::OrderCardWidget(const Order &order, QWidget *parent)
    : QWidget(parent), m_order(order) {
    setupUI();
    
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &OrderCardWidget::updateWaitTime);
    m_timer->start(1000);
    updateWaitTime();
}

OrderCardWidget::~OrderCardWidget() {
    if (m_timer) m_timer->stop();
}

void OrderCardWidget::setupUI() {
    // Dark theme KDS style for high contrast
    setStyleSheet("background-color: #1E1E1E; border: 2px solid #333; border-radius: 8px; padding: 12px;");
    setMinimumWidth(280);
    setMaximumWidth(350);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    // 1. Header: Order Number & Seat
    QHBoxLayout *headerLayout = new QHBoxLayout();
    m_headerLabel = new QLabel(this);
    m_headerLabel->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: bold;");
    
    // Format: ORD-123 | Seat A1
    QString seatInfo = (m_order.seatId > 0) ? QString("Seat %1").arg(m_order.seatId) : "Walk-in";
    m_headerLabel->setText(QString("%1 | %2").arg(m_order.orderNumber).arg(seatInfo));
    headerLayout->addWidget(m_headerLabel);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    // 2. Wait Time
    m_waitTimeLabel = new QLabel(this);
    m_waitTimeLabel->setStyleSheet("color: #FF9800; font-size: 14px; font-weight: bold;");
    mainLayout->addWidget(m_waitTimeLabel);

    // Separator
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #444;");
    mainLayout->addWidget(line);

    // 3. Order Items Details
    m_itemsLayout = new QVBoxLayout();
    m_itemsLayout->setSpacing(6);
    
    for (const auto &item : m_order.items) {
        QHBoxLayout *itemRow = new QHBoxLayout();
        
        // Qty & Name
        QLabel *itemLabel = new QLabel(QString("%1x %2").arg(item.quantity).arg(item.itemName), this);
        itemLabel->setStyleSheet("color: #E0E0E0; font-size: 14px;");
        itemLabel->setWordWrap(true);
        itemRow->addWidget(itemLabel, 1);
        
        m_itemsLayout->addLayout(itemRow);

        // Note (if exists)
        if (!item.note.isEmpty()) {
            QLabel *noteLabel = new QLabel(QString("   ↳ Note: %1").arg(item.note), this);
            noteLabel->setStyleSheet("color: #FF5252; font-size: 12px; font-style: italic;");
            noteLabel->setWordWrap(true);
            m_itemsLayout->addWidget(noteLabel);
        }
    }
    mainLayout->addLayout(m_itemsLayout);

    mainLayout->addStretch();

    // 4. Action Button
    m_actionBtn = new QPushButton(this);
    m_actionBtn->setMinimumHeight(45);
    m_actionBtn->setCursor(Qt::PointingHandCursor);
    m_actionBtn->setStyleSheet("font-size: 16px; font-weight: bold; border-radius: 6px; color: white;");
    
    connect(m_actionBtn, &QPushButton::clicked, this, &OrderCardWidget::handleActionClicked);
    mainLayout->addWidget(m_actionBtn);

    updateActionButton();
}

void OrderCardWidget::updateActionButton() {
    switch (m_order.status) {
        case OrderStatus::Submitted:
        case OrderStatus::Accepted:
            m_actionBtn->setText("🍳 Start Preparing");
            m_actionBtn->setStyleSheet(m_actionBtn->styleSheet() + "background-color: #2196F3;"); // Blue
            break;
        case OrderStatus::Preparing:
            m_actionBtn->setText("✅ Mark Ready");
            m_actionBtn->setStyleSheet(m_actionBtn->styleSheet() + "background-color: #4CAF50;"); // Green
            break;
        case OrderStatus::Ready:
        case OrderStatus::Served:
            m_actionBtn->setText("🗑️ Complete Order");
            m_actionBtn->setStyleSheet(m_actionBtn->styleSheet() + "background-color: #757575;"); // Gray
            break;
        default:
            m_actionBtn->setText("Unknown Status");
            m_actionBtn->setEnabled(false);
            break;
    }
}

void OrderCardWidget::handleActionClicked() {
    OrderStatus nextStatus = m_order.status;
    
    if (m_order.status == OrderStatus::Submitted || m_order.status == OrderStatus::Accepted) {
        nextStatus = OrderStatus::Preparing;
    } else if (m_order.status == OrderStatus::Preparing) {
        nextStatus = OrderStatus::Ready;
    } else if (m_order.status == OrderStatus::Ready || m_order.status == OrderStatus::Served) {
        nextStatus = OrderStatus::Served; // Or Paid/Cancelled depending on your flow
    }

    emit statusChanged(m_order.id, nextStatus);
}

void OrderCardWidget::handleStatusChange(const QString &statusText) {
    OrderStatus newStatus = m_order.status;

    if (statusText == "Submitted") {
        newStatus = OrderStatus::Submitted;
    } else if (statusText == "Accepted") {
        newStatus = OrderStatus::Accepted;
    } else if (statusText == "Preparing") {
        newStatus = OrderStatus::Preparing;
    } else if (statusText == "Ready") {
        newStatus = OrderStatus::Ready;
    } else if (statusText == "Served") {
        newStatus = OrderStatus::Served;
    } else if (statusText == "Paid") {
        newStatus = OrderStatus::Paid;
    } else if (statusText == "Cancelled") {
        newStatus = OrderStatus::Cancelled;
    }

    if (newStatus != m_order.status) {
        emit statusChanged(m_order.id, newStatus);
    }
}

void OrderCardWidget::updateWaitTime() {
    m_waitTimeLabel->setText("⏱️ Waiting: " + calculateWaitTime());
}

QString OrderCardWidget::calculateWaitTime() const {
    qint64 seconds = m_order.createdAt.secsTo(QDateTime::currentDateTime());
    qint64 minutes = seconds / 60;
    qint64 secs = seconds % 60;
    
    // Change color to red if waiting too long (> 15 mins)
    if (minutes >= 15) {
        m_waitTimeLabel->setStyleSheet("color: #F44336; font-size: 14px; font-weight: bold;");
    }
    
    return QString("%1m %2s").arg(minutes, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
}
