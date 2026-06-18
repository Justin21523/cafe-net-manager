#pragma once
#include <QWidget>

class PosMenuPanel;
class PosCartPanel;
class MenuService;
class OrderService;
class MenuItem;

class PosOrderPage : public QWidget {
    Q_OBJECT
public:
    explicit PosOrderPage(QWidget *parent = nullptr);
    void init(MenuService *menuSvc, OrderService *orderSvc);
    
    void setTargetSeat(int seatId, const QString &seatCode);

signals:
    void orderSubmitted();
    void checkoutRequested();

private:
    PosMenuPanel *m_menuPanel;
    PosCartPanel *m_cartPanel;
    OrderService *m_orderService;
    int m_targetSeatId = -1;
};
