#pragma once

#include <QWidget>
#include <qevent.h>
#include "models/MenuItem.h"

class QLabel;
class QPushButton;

class MenuItemCard : public QWidget {
    Q_OBJECT

public:
    explicit MenuItemCard(const MenuItem &item, QWidget *parent = nullptr);

signals:
    void addToCartRequested(const MenuItem &item);

private:
    void setupUI();

    MenuItem m_item;
    QLabel *m_nameLabel;
    QLabel *m_priceLabel;
};