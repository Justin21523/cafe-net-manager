#include "graphics/WallItem.h"
#include <QPainter>

WallItem::WallItem(QGraphicsItem *parent) : FloorPlanItem(parent) {}

QRectF WallItem::boundingRect() const {
    return QRectF(0, 0, m_size.width(), m_size.height());
}

void WallItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option); Q_UNUSED(widget);
    painter->setPen(QPen(isSelected() ? Qt::yellow : Qt::darkGray, isSelected() ? 3 : 1));
    painter->setBrush(QBrush(QColor(120, 120, 120)));
    painter->drawRect(boundingRect());
}

void WallItem::setSize(const QSizeF &size) {
    prepareGeometryChange();
    m_size = size;
    update();
}