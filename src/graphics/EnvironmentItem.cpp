#include "graphics/EnvironmentItem.h"

#include <QPainter>
#include <QPen>
#include <QBrush>

EnvironmentItem::EnvironmentItem(ObjectType type, QGraphicsItem *parent)
    : FloorPlanItem(parent), m_type(type) {
    if (m_type == ObjectType::WALL) {
        setSize(QSizeF(120, 20));
    } else if (m_type == ObjectType::COUNTER) {
        setSize(QSizeF(120, 60));
    } else if (m_type == ObjectType::DOOR || m_type == ObjectType::WINDOW) {
        setSize(QSizeF(80, 20));
    }
}

QString EnvironmentItem::displayName() const {
    switch (m_type) {
        case ObjectType::WALL: return "Wall";
        case ObjectType::COUNTER: return "Counter";
        case ObjectType::DOOR: return "Door";
        case ObjectType::WINDOW: return "Window";
        default: return "Environment Item";
    }
}

void EnvironmentItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor fillColor(180, 180, 180);
    if (m_type == ObjectType::WALL) fillColor = QColor(90, 90, 90);
    if (m_type == ObjectType::COUNTER) fillColor = QColor(120, 90, 60);
    if (m_type == ObjectType::DOOR) fillColor = QColor(160, 110, 60);
    if (m_type == ObjectType::WINDOW) fillColor = QColor(120, 180, 220);

    painter->setPen(QPen(isSelected() ? Qt::yellow : Qt::black, isSelected() ? 2 : 1));
    painter->setBrush(QBrush(fillColor));
    drawShape(painter);
    drawResizeHandles(painter);
}
