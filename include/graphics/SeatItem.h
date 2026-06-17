#pragma once
#include "graphics/FloorPlanItem.h"
#include "models/Seat.h"

class SeatItem : public FloorPlanItem {
    Q_OBJECT
public:
    explicit SeatItem(const Seat &seat, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    ObjectType objectType() const override { return ObjectType::SEAT; }
    QString displayName() const override { return "Seat: " + m_seat.code; }

    const Seat& seatData() const { return m_seat; }
    void updateSeatData(const Seat &seat);

    // Properties for Inspector
    int capacity() const { return m_seat.capacity; }
    void setCapacity(int cap);
    bool hasPowerOutlet() const { return m_seat.hasPowerOutlet; }
    void setHasPowerOutlet(bool has);

signals:
    void dataChanged();

private:
    Seat m_seat;
    QColor getStatusColor() const;
};