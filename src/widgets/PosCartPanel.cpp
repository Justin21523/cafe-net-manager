#include "widgets/PosCartPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QSpinBox>
#include <QPushButton>
#include <QFont>

PosCartPanel::PosCartPanel(OrderService *service, QWidget *parent)
    : QWidget(parent), m_service(service) {
    setupUI();
}

void PosCartPanel::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);
    setStyleSheet("background-color: #f5f5f5; border-left: 1px solid #ddd;");

    // Header: Seat Info
    m_seatInfoLabel = new QLabel("🪑 No Seat Selected", this);
    QFont headerFont = m_seatInfoLabel->font();
    headerFont.setPointSize(14); headerFont.setBold(true);
    m_seatInfoLabel->setFont(headerFont);
    m_seatInfoLabel->setStyleSheet("background-color: #e0e0e0; padding: 10px; border-radius: 4px;");
    layout->addWidget(m_seatInfoLabel);

    // Cart Table
    m_cartTable = new QTableWidget(this);
    m_cartTable->setColumnCount(5);
    m_cartTable->setHorizontalHeaderLabels({"Item", "Note", "Qty", "Price", "Action"});
    m_cartTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_cartTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_cartTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_cartTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_cartTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_cartTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_cartTable->verticalHeader()->setVisible(false);
    layout->addWidget(m_cartTable, 1);

    // Totals
    m_totalLabel = new QLabel("Total: $0.00", this);
    QFont totalFont = m_totalLabel->font();
    totalFont.setPointSize(18); totalFont.setBold(true);
    m_totalLabel->setFont(totalFont);
    m_totalLabel->setAlignment(Qt::AlignRight);
    m_totalLabel->setStyleSheet("color: #d32f2f; padding: 10px;");
    layout->addWidget(m_totalLabel);

    // Action Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    m_sendBtn = new QPushButton("🍳 Send to Kitchen", this);
    m_sendBtn->setMinimumHeight(50);
    m_sendBtn->setStyleSheet("background-color: #2196F3; color: white; font-size: 16px; font-weight: bold; border-radius: 4px;");
    connect(m_sendBtn, &QPushButton::clicked, this, &PosCartPanel::sendToKitchenRequested);
    btnLayout->addWidget(m_sendBtn);

    m_checkoutBtn = new QPushButton("💳 Checkout", this);
    m_checkoutBtn->setMinimumHeight(50);
    m_checkoutBtn->setStyleSheet("background-color: #4CAF50; color: white; font-size: 16px; font-weight: bold; border-radius: 4px;");
    connect(m_checkoutBtn, &QPushButton::clicked, this, &PosCartPanel::checkoutRequested);
    btnLayout->addWidget(m_checkoutBtn);

    layout->addLayout(btnLayout);
}

void PosCartPanel::setTargetSeat(int seatId, const QString &seatCode) {
    m_targetSeatId = seatId;
    if (seatId == -1) {
        m_seatInfoLabel->setText("🪑 No Seat Selected (Walk-in)");
        m_seatInfoLabel->setStyleSheet("background-color: #fff3e0; padding: 10px; border-radius: 4px; color: #e65100;");
    } else {
        m_seatInfoLabel->setText("🪑 Seat: " + seatCode);
        m_seatInfoLabel->setStyleSheet("background-color: #e8f5e9; padding: 10px; border-radius: 4px; color: #2e7d32;");
    }
}

void PosCartPanel::handleAddItem(const MenuItem &item, const QString &note) {
    m_service->addToCart(item, note); // Need to update OrderService signature
    refreshCart();
}

void PosCartPanel::refreshCart() {
    m_cartTable->setRowCount(0);
    auto items = m_service->getCartItems();

    for (int i = 0; i < items.size(); ++i) {
        const auto &cartItem = items[i];
        int row = m_cartTable->rowCount();
        m_cartTable->insertRow(row);

        m_cartTable->setItem(row, 0, new QTableWidgetItem(cartItem.menuItem.name));
        m_cartTable->setItem(row, 1, new QTableWidgetItem(cartItem.note));

        // Qty SpinBox
        QSpinBox *spin = new QSpinBox(this);
        spin->setRange(1, 99);
        spin->setValue(cartItem.quantity);
        connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, i](int val) {
            handleQtyChanged(i, val);
        });
        m_cartTable->setCellWidget(row, 2, spin);

        double price = cartItem.menuItem.price / 100.0;
        m_cartTable->setItem(row, 3, new QTableWidgetItem(QString("$%1").arg(price, 0, 'f', 2)));

        // Delete Button
        QPushButton *delBtn = new QPushButton("🗑️", this);
        delBtn->setStyleSheet("background-color: #f44336; color: white; border-radius: 4px;");
        connect(delBtn, &QPushButton::clicked, this, [this, i]() {
            handleRemoveItem(i);
        });
        m_cartTable->setCellWidget(row, 4, delBtn);
    }
    updateTotals();
}

void PosCartPanel::handleQtyChanged(int row, int value) {
    auto items = m_service->getCartItems();
    if (row < items.size()) {
        m_service->updateQuantity(items[row].menuItem.id, value);
        refreshCart();
    }
}

void PosCartPanel::handleRemoveItem(int row) {
    auto items = m_service->getCartItems();
    if (row < items.size()) {
        m_service->removeFromCart(items[row].menuItem.id);
        refreshCart();
    }
}

void PosCartPanel::updateTotals() {
    int totalCents = m_service->calculateCartTotal();
    m_totalLabel->setText(QString("Total: $%1").arg(totalCents / 100.0, 0, 'f', 2));
    
    bool hasItems = (totalCents > 0);
    m_sendBtn->setEnabled(hasItems && m_targetSeatId != -1);
    m_checkoutBtn->setEnabled(hasItems);
}