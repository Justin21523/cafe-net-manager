#pragma once
#include <QGraphicsPathItem>

class LineItem : public QGraphicsPathItem {
public:
    LineItem(const QPointF &start, const QPointF &end, QGraphicsItem *parent = nullptr);
    void updateLine(const QPointF &end);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
private:
    QPointF m_start, m_end;
};