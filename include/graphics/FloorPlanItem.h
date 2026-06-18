#pragma once
#include <QGraphicsObject>

enum class ObjectType { SEAT, WALL, COUNTER, DOOR, WINDOW, LINE };
enum class ShapeType { Rectangle, Circle, Triangle };

class FloorPlanItem : public QGraphicsObject {
    Q_OBJECT
public:
    explicit FloorPlanItem(QGraphicsItem *parent = nullptr);
    virtual ~FloorPlanItem();

    virtual ObjectType objectType() const = 0;
    virtual QString displayName() const = 0;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QSizeF size() const { return m_size; }
    void setSize(const QSizeF &size);

    ShapeType shapeType() const { return m_shapeType; }
    void setShapeType(ShapeType type);

    void setEditMode(bool enabled);
    bool isEditMode() const { return m_isEditMode; }

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void geometryChanged();
    void itemDeleteRequested(FloorPlanItem *item);

protected:
    void drawShape(QPainter *painter);
    void drawResizeHandles(QPainter *painter);
    int snapToGrid(qreal value) const;
    int getHandleAtPos(const QPointF &pos) const;

    QSizeF m_size;
    ShapeType m_shapeType = ShapeType::Rectangle;
    bool m_isEditMode = false;

private:
    int m_draggedHandle = -1; // -1=none, 0=TL, 1=TR, 2=BL, 3=BR, 4=body
    QPointF m_dragStartScenePos;
    QSizeF m_dragStartSize;
    QPointF m_dragStartItemPos;

    static constexpr int HANDLE_SIZE = 8;
    static constexpr int HANDLE_HIT_MARGIN = 6;
};