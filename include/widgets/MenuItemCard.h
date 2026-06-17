#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <qevent.h>
#include "models/MenuItem.h"

class QLabel;

class MenuItemCard : public QWidget {
    Q_OBJECT

public:
    explicit MenuItemCard(const MenuItem &item, QWidget *parent = nullptr);

signals:
    void addToCartRequested(const MenuItem &item);
    void mousePressEvent(QMouseEvent *event);

private:
    void setupUI();

    MenuItem m_item;
    QLabel *m_nameLabel;
    QLabel *m_priceLabel;
};