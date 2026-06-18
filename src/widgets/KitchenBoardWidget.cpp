#include "widgets/KitchenBoardWidget.h"
#include "widgets/OrderCardWidget.h"
#include "services/OrderService.h"
#include "utils/Logger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>

KitchenBoardWidget::KitchenBoardWidget(OrderService *service, QWidget *parent)
    : QWidget(parent), m_service(service) {
    setupUI();
    if (m_service) {
        refreshBoard();
    }
}

void KitchenBoardWidget::setupUI() {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(10);

    // Helper lambda to create a column
    auto createColumn = [&](const QString &title, QVBoxLayout *&layout) {
        QWidget *columnWidget = new QWidget(this);
        columnWidget->setStyleSheet("background-color: #2b2b2b; border-radius: 5px;");
        QVBoxLayout *colLayout = new QVBoxLayout(columnWidget);
        colLayout->setContentsMargins(5, 5, 5, 5);
        
        QLabel *titleLabel = new QLabel(title, columnWidget);
        QFont font = titleLabel->font();
        font.setPointSize(12);
        font.setBold(true);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("color: #FFFFFF; background-color: #333333; padding: 10px; border-radius: 4px; font-size: 16px;");
        colLayout->addWidget(titleLabel);

        QScrollArea *scrollArea = new QScrollArea(columnWidget);
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setStyleSheet("background-color: transparent;");
        
        QWidget *scrollContent = new QWidget();
        layout = new QVBoxLayout(scrollContent);
        layout->setSpacing(10);
        layout->addStretch();
        
        scrollArea->setWidget(scrollContent);
        colLayout->addWidget(scrollArea);

        mainLayout->addWidget(columnWidget, 1); // Equal stretch
    };

    createColumn("Pending", m_pendingLayout);
    createColumn("Preparing", m_preparingLayout);
    createColumn("Ready", m_readyLayout);
}

void KitchenBoardWidget::refreshBoard() {
    clearColumns();
    if (!m_service) return;

    std::vector<Order> orders = m_service->getActiveOrders();

    for (auto &order : orders) { // Use reference so we can modify it
        // Fetch items for this order
        order.items = m_service->getOrderItems(order.id);

        OrderCardWidget *card = new OrderCardWidget(order);
        connect(card, &OrderCardWidget::statusChanged, this, &KitchenBoardWidget::handleOrderStatusChanged);

        QVBoxLayout *targetLayout = nullptr;
        if (order.status == OrderStatus::Draft || order.status == OrderStatus::Submitted || order.status == OrderStatus::Accepted) {
            targetLayout = m_pendingLayout;
        } else if (order.status == OrderStatus::Preparing) {
            targetLayout = m_preparingLayout;
        } else if (order.status == OrderStatus::Ready || order.status == OrderStatus::Served) {
            targetLayout = m_readyLayout;
        }

        if (targetLayout) {
            targetLayout->insertWidget(targetLayout->count() - 1, card);
        }
    }
}

void KitchenBoardWidget::clearColumns() {
    auto clearLayout = [](QVBoxLayout *layout) {
        // Delete all OrderCardWidgets
        QList<OrderCardWidget*> cards = layout->parentWidget()->findChildren<OrderCardWidget*>();
        for (OrderCardWidget *card : cards) {
            delete card;
        }
    };

    clearLayout(m_pendingLayout);
    clearLayout(m_preparingLayout);
    clearLayout(m_readyLayout);
}

void KitchenBoardWidget::handleOrderStatusChanged(int orderId, OrderStatus newStatus) {
    Logger::info("Order " + QString::number(orderId) + " status changed to " + QString::number(static_cast<int>(newStatus)));
    if (m_service) {
        if (m_service->updateOrderStatus(orderId, newStatus)) {
            refreshBoard();
        }
    }
}