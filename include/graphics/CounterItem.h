#pragma once
#include "graphics/FloorPlanItem.h"

class CounterItem : public FloorPlanItem {
    Q_OBJECT
public:
    explicit CounterItem(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    ObjectType objectType() const override { return ObjectType::COUNTER; }
    QString displayName() const override { return "Counter"; }

    QSizeF size() const { return m_size; }
    void setSize(const QSizeF &size);

private:
    QSizeF m_size = QSizeF(120, 60);
};