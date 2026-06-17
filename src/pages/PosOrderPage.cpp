#include "pages/PosOrderPage.h"
#include "widgets/MenuBrowserWidget.h"
#include "widgets/CartWidget.h"
#include "services/MenuService.h"
#include "services/OrderService.h"
#include <QSplitter>
#include <QVBoxLayout>

PosOrderPage::PosOrderPage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
}

void PosOrderPage::init(MenuService *menuService, OrderService *orderService) {
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    
    m_menuBrowser = new MenuBrowserWidget(menuService, splitter);
    m_cartWidget = new CartWidget(orderService, splitter);
    
    splitter->addWidget(m_menuBrowser);
    splitter->addWidget(m_cartWidget);
    splitter->setSizes({700, 400});
    
    this->layout()->addWidget(splitter);
    
    connect(m_menuBrowser, &MenuBrowserWidget::itemAddedToCart, this, &PosOrderPage::itemAddedToCart);
    connect(m_cartWidget, &CartWidget::orderSubmitted, this, &PosOrderPage::orderSubmitted);
}

void PosOrderPage::setSelectedSeat(int seatId, int sessionId) {
    if (m_cartWidget) m_cartWidget->setSelectedSeat(seatId, sessionId);
}