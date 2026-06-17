#include "graphics/CounterItem.h"
#include <QPainter>
#include <qnamespace.h>

CounterItem::CounterItem(QGraphicsItem *parent) : FloorPlanItem(parent) {}

QRectF CounterItem::boundingRect() const {
    return QRectF(0, 0, m_size.width(), m_size.height());
}

void CounterItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option); Q_UNUSED(widget);
    painter->setPen(QPen(isSelected() ? Qt::yellow : Qt::darkGreen, isSelected() ? 3 : 1));
    painter->setBrush(QBrush(QColor(139, 69, 19))); // Brown color
    painter->drawRect(boundingRect());
    
    // Draw text
    painter->setPen(Qt::white);
    painter->drawText(boundingRect(), Qt::AlignCenter, "Counter");
}

void CounterItem::setSize(const QSizeF &size) {
    prepareGeometryChange();
    m_size = size;
    update();
}