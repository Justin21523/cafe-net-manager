#pragma once
#include <QWidget>

class KitchenBoardWidget;
class OrderService;

class KitchenPage : public QWidget {
    Q_OBJECT
public:
    explicit KitchenPage(QWidget *parent = nullptr);
    void init(OrderService *orderService);
    void refreshBoard();

private:
    KitchenBoardWidget* m_kitchenBoard = nullptr;
};