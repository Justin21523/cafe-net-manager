#include "graphics/LineItem.h"
#include <QPainter>
#include <QPainterPathStroker>

LineItem::LineItem(const QPointF &start, const QPointF &end, QGraphicsItem *parent)
    : QGraphicsPathItem(parent), m_start(start), m_end(end) {
    setPen(QPen(Qt::darkGray, 3));
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    updateEndPoint(end);
}

void LineItem::updateEndPoint(const QPointF &end) {
    m_end = end;
    QPainterPath path;
    path.moveTo(m_start);
    path.lineTo(m_end);
    setPath(path);
}

QRectF LineItem::boundingRect() const {
    return QRectF(m_start, m_end).normalized().adjusted(-10, -10, 10, 10);
}

QPainterPath LineItem::shape() const {
    QPainterPathStroker stroker;
    stroker.setWidth(10);
    return stroker.createStroke(path());
}

void LineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget);
    QPen p = pen();
    if (isSelected()) {
        p.setColor(Qt::yellow);
        p.setWidth(4);
    }
    painter->setPen(p);
    painter->drawLine(m_start, m_end);

    // Draw endpoints
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(m_start, 4, 4);
    painter->drawEllipse(m_end, 4, 4);
}