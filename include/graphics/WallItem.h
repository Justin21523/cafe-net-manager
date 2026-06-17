#pragma once
#include "graphics/FloorPlanItem.h"

class WallItem : public FloorPlanItem {
    Q_OBJECT
public:
    explicit WallItem(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    ObjectType objectType() const override { return ObjectType::WALL; }
    QString displayName() const override { return "Wall"; }

    QSizeF size() const { return m_size; }
    void setSize(const QSizeF &size);

private:
    QSizeF m_size = QSizeF(100, 20); // Default wall size
};