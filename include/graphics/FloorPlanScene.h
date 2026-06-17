#pragma once
#include <QGraphicsScene>

class FloorPlanScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit FloorPlanScene(QObject *parent = nullptr);

    void setGridSize(int size);
    int gridSize() const;

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    int m_gridSize = 20;
};