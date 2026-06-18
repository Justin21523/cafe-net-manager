#include "graphics/LineItem.h"
#include <QPainter>

LineItem::LineItem(const QPointF &start, const QPointF &end, QGraphicsItem *parent)
    : QGraphicsPathItem(parent), m_start(start), m_end(end) {
    setPen(QPen(Qt::black, 2));
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    updateLine(end);
}

void LineItem::updateLine(const QPointF &end) {
    m_end = end;
    QPainterPath path;
    path.moveTo(m_start);
    path.lineTo(m_end);
    setPath(path);
}

QRectF LineItem::boundingRect() const {
    return QRectF(m_start, m_end).normalized().adjusted(-5, -5, 5, 5);
}

void LineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget);
    if (isSelected()) {
        painter->setPen(QPen(Qt::yellow, 3));
    } else {
        painter->setPen(QPen(Qt::black, 2));
    }
    painter->drawLine(m_start, m_end);
}