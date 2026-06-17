#include "graphics/FloorPlanItem.h"
#include "graphics/FloorPlanScene.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QBrush>
#include <QCursor>

FloorPlanItem::FloorPlanItem(QGraphicsItem *parent) : QGraphicsObject(parent) {
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    
    m_size = QSizeF(100, 100);

    // Create 4 resize handles (Top-Left, Top-Right, Bottom-Left, Bottom-Right)
    for (int i = 0; i < 4; ++i) {
        m_handles[i] = new QGraphicsRectItem(-5, -5, 10, 10, this);
        m_handles[i]->setBrush(QBrush(Qt::blue));
        m_handles[i]->setPen(QPen(Qt::white, 1));
        m_handles[i]->setFlag(QGraphicsItem::ItemIsMovable, false);
        m_handles[i]->setVisible(false); // Hidden by default
        m_handles[i]->setCursor(Qt::SizeFDiagCursor);
        m_handles[i]->setZValue(1); // Above the item
    }
}

FloorPlanItem::~FloorPlanItem() {}

QRectF FloorPlanItem::boundingRect() const {
    return QRectF(0, 0, m_size.width(), m_size.height());
}

void FloorPlanItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option); Q_UNUSED(widget);
    
    QColor fillColor = (objectType() == ObjectType::SEAT) ? QColor(144, 238, 144) : 
                       (objectType() == ObjectType::WALL) ? QColor(120, 120, 120) : QColor(139, 69, 19);
    QColor borderColor = isSelected() ? Qt::yellow : Qt::black;
    
    painter->setPen(QPen(borderColor, isSelected() ? 3 : 1));
    painter->setBrush(QBrush(fillColor));

    if (m_shapeType == ShapeType::Rectangle) {
        painter->drawRect(boundingRect());
    } else if (m_shapeType == ShapeType::Circle) {
        painter->drawEllipse(boundingRect());
    } else if (m_shapeType == ShapeType::Triangle) {
        QPolygonF polygon;
        polygon << boundingRect().topLeft() 
                << QPointF(boundingRect().right(), boundingRect().center().y()) // Mid-right for a right triangle, or adjust as needed
                << boundingRect().bottomLeft();
        // Let's draw an equilateral-ish triangle
        QPolygonF tri;
        tri << QPointF(boundingRect().center().x(), boundingRect().top())
            << boundingRect().bottomRight()
            << boundingRect().bottomLeft();
        painter->drawPolygon(tri);
    }

    // Draw Text
    painter->setPen(Qt::black);
    painter->drawText(boundingRect(), Qt::AlignCenter, displayName());
}

void FloorPlanItem::setSize(const QSizeF &size) {
    if (m_size != size) {
        prepareGeometryChange();
        m_size = size;
        updateHandlePositions();
        update();
        emit geometryChanged();
    }
}

void FloorPlanItem::setShapeType(ShapeType type) {
    if (m_shapeType != type) {
        m_shapeType = type;
        update();
        emit geometryChanged();
    }
}

void FloorPlanItem::setEditMode(bool enabled) {
    m_isEditMode = enabled;
    setFlag(QGraphicsItem::ItemIsMovable, enabled);
    
    // Show/hide handles
    for (int i = 0; i < 4; ++i) {
        m_handles[i]->setVisible(enabled && isSelected());
    }
    updateHandlePositions();
}

QVariant FloorPlanItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged && m_isEditMode) {
        QPointF newPos = value.toPointF();
        newPos.setX(snapToGrid(newPos.x()));
        newPos.setY(snapToGrid(newPos.y()));
        return newPos;
    }
    if (change == ItemSelectedHasChanged) {
        if (m_isEditMode) {
            bool selected = value.toBool();
            for (int i = 0; i < 4; ++i) m_handles[i]->setVisible(selected);
        }
    }
    if (change == ItemPositionHasChanged) {
        emit geometryChanged();
    }
    return QGraphicsObject::itemChange(change, value);
}

void FloorPlanItem::updateHandlePositions() {
    qreal w = m_size.width();
    qreal h = m_size.height();
    m_handles[0]->setPos(0, 0);             // Top-Left
    m_handles[1]->setPos(w, 0);             // Top-Right
    m_handles[2]->setPos(0, h);             // Bottom-Left
    m_handles[3]->setPos(w, h);             // Bottom-Right
}

int FloorPlanItem::snapToGrid(int value) const {
    int gridSize = 20;
    if (scene()) {
        FloorPlanScene *fpScene = qobject_cast<FloorPlanScene *>(scene());
        if (fpScene) {
            gridSize = fpScene->gridSize();
        }
    }
    if (gridSize <= 0) {
        return value;
    }
    return static_cast<int>(
        std::round(static_cast<double>(value) / static_cast<double>(gridSize))
    ) * gridSize;
}