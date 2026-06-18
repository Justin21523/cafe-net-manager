#include "pages/PosOrderPage.h"
#include "widgets/PosMenuPanel.h"
#include "widgets/PosCartPanel.h"
#include "services/MenuService.h"
#include "services/OrderService.h"

#include <QHBoxLayout>

PosOrderPage::PosOrderPage(QWidget *parent) : QWidget(parent) {}

void PosOrderPage::init(MenuService *menuSvc, OrderService *orderSvc) {
    m_orderService = orderSvc;
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_menuPanel = new PosMenuPanel(menuSvc, this);
    m_cartPanel = new PosCartPanel(orderSvc, this);

    layout->addWidget(m_menuPanel, 7); // 70% width
    layout->addWidget(m_cartPanel, 3); // 30% width

    // Connect signals
    connect(m_menuPanel, &PosMenuPanel::itemAddRequested, 
            m_cartPanel, &PosCartPanel::handleAddItem);
    
    connect(m_cartPanel, &PosCartPanel::sendToKitchenRequested, this, [this]() {
        if (m_orderService->submitOrder(m_targetSeatId, -1)) {
            emit orderSubmitted();
            m_cartPanel->refreshCart();
        }
    });
    
    connect(m_cartPanel, &PosCartPanel::checkoutRequested, this, &PosOrderPage::checkoutRequested);
}

void PosOrderPage::setTargetSeat(int seatId, const QString &seatCode) {
    m_targetSeatId = seatId;
    if (m_cartPanel) {
        m_cartPanel->setTargetSeat(seatId, seatCode);
    }
}
