#pragma once

#include <QGraphicsObject>
#include <QGraphicsRectItem>

enum class ObjectType { SEAT, WALL, COUNTER, DOOR, WINDOW };
enum class ShapeType { Rectangle, Circle, Triangle };

class FloorPlanItem : public QGraphicsObject {
    Q_OBJECT

public:
    explicit FloorPlanItem(QGraphicsItem *parent = nullptr);
    virtual ~FloorPlanItem();

    virtual ObjectType objectType() const = 0;
    virtual QString displayName() const = 0;

    // Geometry & Shape
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    QSizeF size() const { return m_size; }
    void setSize(const QSizeF &size);
    
    ShapeType shapeType() const { return m_shapeType; }
    void setShapeType(ShapeType type);

    // Edit Mode & Handles
    void setEditMode(bool enabled);
    bool isEditMode() const { return m_isEditMode; }

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

signals:
    void geometryChanged(); // Emitted when size or pos changes

protected:
    void updateHandlePositions();
    int snapToGrid(int value) const;

    QSizeF m_size;
    ShapeType m_shapeType = ShapeType::Rectangle;
    bool m_isEditMode = false;

private:
    QGraphicsRectItem *m_handles[4];
    int m_draggedHandle = -1;
    QPointF m_dragStartPos;
    QSizeF m_dragStartSize;
};