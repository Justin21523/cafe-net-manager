#include "graphics/SeatItem.h"
#include <QPainter>
#include <QFont>

SeatItem::SeatItem(const Seat &seat, QGraphicsItem *parent) 
    : FloorPlanItem(parent), m_seat(seat) {
    setPos(m_seat.x, m_seat.y);
}

QRectF SeatItem::boundingRect() const {
    return QRectF(0, 0, m_seat.width, m_seat.height);
}

void SeatItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option); Q_UNUSED(widget);
    
    QColor fillColor = getStatusColor();
    QColor borderColor = isSelected() ? Qt::yellow : Qt::black;
    int borderWidth = isSelected() ? 3 : 1;

    painter->setPen(QPen(borderColor, borderWidth));
    painter->setBrush(QBrush(fillColor));
    painter->drawRect(boundingRect());

    // Draw Text
    painter->setPen(Qt::black);
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(10);
    painter->setFont(font);
    painter->drawText(boundingRect(), Qt::AlignCenter, m_seat.code);
}

void SeatItem::updateSeatData(const Seat &seat) {
    m_seat = seat;
    update();
}

void SeatItem::setCapacity(int cap) {
    if (m_seat.capacity != cap) {
        m_seat.capacity = cap;
        emit dataChanged();
    }
}

void SeatItem::setHasPowerOutlet(bool has) {
    if (m_seat.hasPowerOutlet != has) {
        m_seat.hasPowerOutlet = has;
        emit dataChanged();
    }
}

QColor SeatItem::getStatusColor() const {
    switch (m_seat.status) {
        case SeatStatus::Available: return QColor(144, 238, 144);
        case SeatStatus::Occupied:  return QColor(255, 99, 71);
        case SeatStatus::Reserved:  return QColor(135, 206, 235);
        case SeatStatus::Cleaning:  return QColor(169, 169, 169);
        case SeatStatus::Disabled:  return QColor(105, 105, 105);
        default: return Qt::white;
    }
}