#pragma once

#include <QGraphicsObject>
#include "models/Seat.h"

class SeatGraphicsItem : public QGraphicsObject {
    Q_OBJECT

public:
    explicit SeatGraphicsItem(const Seat &seat, QGraphicsItem *parent = nullptr);

    // Required overrides for QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    const Seat& seatData() const;
    void updateStatus(SeatStatus newStatus);
    void setEditMode(bool enabled);
    bool isEditMode() const;

private:
    Seat m_seat;
    bool m_isEditMode = false;    
    QColor getStatusColor(SeatStatus status) const;
};