#pragma once
#include <QGraphicsPathItem>

class LineItem : public QGraphicsPathItem {
public:
    LineItem(const QPointF &start, const QPointF &end, QGraphicsItem *parent = nullptr);
    void updateEndPoint(const QPointF &end);
    void updateLine(const QPointF &end) { updateEndPoint(end); }
    QPointF startPoint() const { return m_start; }
    QPointF endPoint() const { return m_end; }
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPainterPath shape() const override; // For better hit detection
private:
    QPointF m_start, m_end;
};
