#include "pages/KitchenPage.h"
#include "widgets/KitchenBoardWidget.h"
#include "services/OrderService.h"
#include <QVBoxLayout>

KitchenPage::KitchenPage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
}

void KitchenPage::init(OrderService *orderService) {
    m_kitchenBoard = new KitchenBoardWidget(orderService, this);
    this->layout()->addWidget(m_kitchenBoard);
}

void KitchenPage::refreshBoard() {
    if (m_kitchenBoard) m_kitchenBoard->refreshBoard();
}