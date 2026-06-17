#pragma once
#include <QWidget>
#include "models/MenuItem.h"

class MenuBrowserWidget;
class CartWidget;
class MenuService;
class OrderService;

class PosOrderPage : public QWidget {
    Q_OBJECT
public:
    explicit PosOrderPage(QWidget *parent = nullptr);
    void init(MenuService *menuService, OrderService *orderService);
    void setSelectedSeat(int seatId, int sessionId);

signals:
    void itemAddedToCart(const MenuItem &item);
    void orderSubmitted();

private:
    MenuBrowserWidget* m_menuBrowser = nullptr;
    CartWidget* m_cartWidget = nullptr;
};