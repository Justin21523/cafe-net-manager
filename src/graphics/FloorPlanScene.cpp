#include "graphics/FloorPlanScene.h"
#include <QPainter>
#include <QPen>

FloorPlanScene::FloorPlanScene(QObject *parent) : QGraphicsScene(parent) {
    setSceneRect(0, 0, 1000, 800);
}

void FloorPlanScene::setGridSize(int size) { m_gridSize = size; }
int FloorPlanScene::gridSize() const { return m_gridSize; }

void FloorPlanScene::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsScene::drawBackground(painter, rect);

    // Draw Grid
    QPen gridPen(QColor(200, 200, 200));
    painter->setPen(gridPen);

    qreal left = int(rect.left()) - (int(rect.left()) % m_gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % m_gridSize);

    // Vertical lines
    for (qreal x = left; x < rect.right(); x += m_gridSize) {
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }
    // Horizontal lines
    for (qreal y = top; y < rect.bottom(); y += m_gridSize) {
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }
}