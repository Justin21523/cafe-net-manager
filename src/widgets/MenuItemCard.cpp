#include "widgets/MenuItemCard.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

MenuItemCard::MenuItemCard(const MenuItem &item, QWidget *parent)
    : QWidget(parent), m_item(item) {
    setupUI();
    
    // Make the whole card clickable to add to cart
    setCursor(Qt::PointingHandCursor);
}

void MenuItemCard::setupUI() {  
    setStyleSheet("background-color: #f9f9f9; border: 1px solid #ddd; border-radius: 5px;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);

    m_nameLabel = new QLabel(m_item.name, this);
    QFont nameFont = m_nameLabel->font();
    nameFont.setBold(true);
    m_nameLabel->setFont(nameFont);
    layout->addWidget(m_nameLabel);

    double priceDollars = m_item.price / 100.0;
    m_priceLabel = new QLabel(QString("$%1").arg(priceDollars, 0, 'f', 2), this);
    m_priceLabel->setStyleSheet("color: green; font-weight: bold;");
    layout->addWidget(m_priceLabel);

    QPushButton *addBtn = new QPushButton("Add", this);
    layout->addWidget(addBtn);

    // Connect button click to signal
    connect(addBtn, &QPushButton::clicked, this, [this]() {
        emit addToCartRequested(m_item);
    });
}

