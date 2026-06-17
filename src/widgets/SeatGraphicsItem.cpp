#include "widgets/SeatGraphicsItem.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>

SeatGraphicsItem::SeatGraphicsItem(const Seat &seat, QGraphicsItem *parent)
    : QGraphicsObject(parent), m_seat(seat) {
    // Enable selection and focus for mouse interaction
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    
    // Set position and size based on seat data
    setPos(m_seat.x, m_seat.y);
}

QRectF SeatGraphicsItem::boundingRect() const {
    // Define the clickable/drawable area
    return QRectF(0, 0, m_seat.width, m_seat.height);
}

void SeatGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor fillColor = getStatusColor(m_seat.status);
    QColor borderColor = Qt::black;

    // Change border color if selected
    if (isSelected()) {
        borderColor = Qt::yellow;
        painter->setPen(QPen(borderColor, 3));
    } else {
        painter->setPen(QPen(borderColor, 1));
    }

    painter->setBrush(QBrush(fillColor));
    painter->drawRect(boundingRect());

    // Draw seat code in the center
    painter->setPen(Qt::black);
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(12);
    painter->setFont(font);
    painter->drawText(boundingRect(), Qt::AlignCenter, m_seat.code);
}

const Seat& SeatGraphicsItem::seatData() const {
    return m_seat;
}

void SeatGraphicsItem::updateStatus(SeatStatus newStatus) {
    if (m_seat.status != newStatus) {
        m_seat.status = newStatus;
        update(); // Trigger repaint
    }
}

QColor SeatGraphicsItem::getStatusColor(SeatStatus status) const {
    switch (status) {
        case SeatStatus::Available: return QColor(144, 238, 144); // Light Green
        case SeatStatus::Occupied:  return QColor(255, 99, 71);   // Tomato Red
        case SeatStatus::Reserved:  return QColor(135, 206, 235); // Sky Blue
        case SeatStatus::Cleaning:  return QColor(169, 169, 169); // Dark Gray
        case SeatStatus::Disabled:  return QColor(105, 105, 105); // Dim Gray
        default: return Qt::white;
    }
}