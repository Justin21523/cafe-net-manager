#include "widgets/CartWidget.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include "utils/Logger.h"

CartWidget::CartWidget(OrderService *service, QWidget *parent)
    : QWidget(parent), m_service(service) {
    setupUI();
    refreshCart();
}

void CartWidget::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Shopping Cart", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    m_cartTable = new QTableWidget(this);
    m_cartTable->setColumnCount(4);
    m_cartTable->setHorizontalHeaderLabels({"Item", "Qty", "Price", "Action"});
    m_cartTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_cartTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_cartTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_cartTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    layout->addWidget(m_cartTable);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    m_totalLabel = new QLabel("Total: $0.00", this);
    m_totalLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    bottomLayout->addWidget(m_totalLabel);
    bottomLayout->addStretch();

    m_submitBtn = new QPushButton("Submit Order", this);
    m_submitBtn->setEnabled(false);
    bottomLayout->addWidget(m_submitBtn);
    layout->addLayout(bottomLayout);

    connect(m_submitBtn, &QPushButton::clicked, this, &CartWidget::handleSubmitOrder);
}

void CartWidget::refreshCart() {
    m_cartTable->setRowCount(0);

    std::vector<CartItem> items = m_service->getCartItems();
    int total = m_service->calculateCartTotal();

    for (int i = 0; i < items.size(); ++i) {
        const auto &item = items[i];
        m_cartTable->insertRow(i);

        // Item Name
        m_cartTable->setItem(i, 0, new QTableWidgetItem(item.menuItem.name));

        // Quantity SpinBox
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setRange(1, 99);
        spinBox->setValue(item.quantity);
        spinBox->setProperty("row", i); // Store row index
        
        // 使用 lambda 連接，捕獲 menuItemId
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
                this, [this, itemId = item.menuItem.id](int value) {
            m_service->updateQuantity(itemId, value);
            refreshCart();
        });
        
        m_cartTable->setCellWidget(i, 1, spinBox);

        // Price
        double price = item.menuItem.price / 100.0;
        m_cartTable->setItem(i, 2, new QTableWidgetItem(QString("$%1").arg(price, 0, 'f', 2)));

        // Remove Button
        QPushButton *removeBtn = new QPushButton("Remove", this);
        connect(removeBtn, &QPushButton::clicked, this, [this, itemId = item.menuItem.id]() {
            m_service->removeFromCart(itemId);
            refreshCart();
        });
        m_cartTable->setCellWidget(i, 3, removeBtn);
    }

    double totalDollars = total / 100.0;
    m_totalLabel->setText(QString("Total: $%1").arg(totalDollars, 0, 'f', 2));
    m_submitBtn->setEnabled(!items.empty());
}

void CartWidget::setSelectedSeat(int seatId, int sessionId) {
    m_currentSeatId = seatId;
    m_currentSessionId = sessionId;
}

void CartWidget::handleSubmitOrder() {
    Logger::info("Submit order clicked. Seat ID: " + QString::number(m_currentSeatId));
    if (m_currentSeatId != -1) {
        if (m_service->submitOrder(m_currentSeatId, m_currentSessionId)) {
            emit orderSubmitted();
            refreshCart();
        } else {
            Logger::error("Failed to submit order");
        }
    } else {
        Logger::warning("No seat selected for order");
    }
}

void CartWidget::handleQuantityChanged(int row) {
    // Simplified: just refresh for now
    refreshCart();
}

void CartWidget::handleRemoveItem() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int row = -1;
    for (int i = 0; i < m_cartTable->rowCount(); ++i) {
        if (m_cartTable->cellWidget(i, 3) == btn) {
            row = i;
            break;
        }
    }

    if (row != -1) {
        std::vector<CartItem> items = m_service->getCartItems();
        if (row < items.size()) {
            m_service->removeFromCart(items[row].menuItem.id);
            refreshCart();
        }
    }
}

