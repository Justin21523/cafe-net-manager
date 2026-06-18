#include "graphics/FloorPlanItem.h"
#include "graphics/FloorPlanScene.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QBrush>
#include <QCursor>

FloorPlanItem::FloorPlanItem(QGraphicsItem *parent) : QGraphicsObject(parent) {
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemIsMovable, false); // Always start locked
    m_size = QSizeF(100, 100);
}

FloorPlanItem::~FloorPlanItem() {}

QRectF FloorPlanItem::boundingRect() const {
    // Extra margin for resize handles
    int margin = m_isEditMode ? HANDLE_SIZE + 2 : 0;
    return QRectF(-margin, -margin, m_size.width() + 2 * margin, m_size.height() + 2 * margin);
}

void FloorPlanItem::drawShape(QPainter *painter) {
    QRectF rect(0, 0, m_size.width(), m_size.height());
    if (m_shapeType == ShapeType::Rectangle) {
        painter->drawRect(rect);
    } else if (m_shapeType == ShapeType::Circle) {
        painter->drawEllipse(rect);
    } else if (m_shapeType == ShapeType::Triangle) {
        QPolygonF tri;
        tri << QPointF(rect.center().x(), rect.top())
            << rect.bottomRight()
            << rect.bottomLeft();
        painter->drawPolygon(tri);
    }
}

void FloorPlanItem::drawResizeHandles(QPainter *painter) {
    if (!m_isEditMode || !isSelected()) return;

    painter->setPen(QPen(Qt::white, 1));
    painter->setBrush(QBrush(Qt::blue));

    qreal w = m_size.width();
    qreal h = m_size.height();
    int hs = HANDLE_SIZE;

    // 4 corners: TL, TR, BL, BR
    QPointF corners[] = {
        QPointF(0, 0), QPointF(w, 0),
        QPointF(0, h), QPointF(w, h)
    };

    for (int i = 0; i < 4; ++i) {
        painter->drawRect(QRectF(corners[i].x() - hs/2, corners[i].y() - hs/2, hs, hs));
    }
}

void FloorPlanItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option); Q_UNUSED(widget);

    QColor borderColor = isSelected() ? Qt::yellow : Qt::black;
    painter->setPen(QPen(borderColor, isSelected() ? 2 : 1));
    painter->setBrush(QBrush(QColor(200, 200, 200)));
    drawShape(painter);
    drawResizeHandles(painter);
}

void FloorPlanItem::setSize(const QSizeF &size) {
    if (m_size != size) {
        prepareGeometryChange();
        m_size = size;
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
    if (!enabled) {
        setSelected(false);
    }
    update();
}

QVariant FloorPlanItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && m_isEditMode) {
        QPointF newPos = value.toPointF();
        newPos.setX(snapToGrid(newPos.x()));
        newPos.setY(snapToGrid(newPos.y()));
        emit geometryChanged();
        return newPos;
    }
    if (change == ItemSelectedHasChanged) {
        update();
    }
    return QGraphicsObject::itemChange(change, value);
}

int FloorPlanItem::getHandleAtPos(const QPointF &pos) const {
    if (!m_isEditMode || !isSelected()) return -1;

    qreal w = m_size.width();
    qreal h = m_size.height();
    int margin = HANDLE_HIT_MARGIN;

    QPointF corners[] = {
        QPointF(0, 0), QPointF(w, 0),
        QPointF(0, h), QPointF(w, h)
    };

    for (int i = 0; i < 4; ++i) {
        if (qAbs(pos.x() - corners[i].x()) <= margin && qAbs(pos.y() - corners[i].y()) <= margin) {
            return i;
        }
    }
    return -1;
}

void FloorPlanItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        QGraphicsObject::mousePressEvent(event);
        return;
    }

    if (m_isEditMode) {
        int handle = getHandleAtPos(event->pos());
        if (handle >= 0) {
            m_draggedHandle = handle;
            m_dragStartScenePos = event->scenePos();
            m_dragStartSize = m_size;
            m_dragStartItemPos = pos();
            event->accept();
            return;
        }
        // Clicked on body - let default move logic handle it
        m_draggedHandle = 4; // body
        m_dragStartScenePos = event->scenePos();
        QGraphicsObject::mousePressEvent(event);
    } else {
        // Preview mode - don't move, just select
        QGraphicsObject::mousePressEvent(event);
    }
}

void FloorPlanItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (!m_isEditMode || m_draggedHandle < 0 || m_draggedHandle == 4) {
        QGraphicsObject::mouseMoveEvent(event);
        return;
    }

    QPointF delta = event->scenePos() - m_dragStartScenePos;
    QSizeF newSize = m_dragStartSize;
    QPointF newPos = m_dragStartItemPos;

    // Handle resize based on which corner is dragged
    switch (m_draggedHandle) {
        case 0: // Top-Left
            newSize.setWidth(qMax(20.0, m_dragStartSize.width() - delta.x()));
            newSize.setHeight(qMax(20.0, m_dragStartSize.height() - delta.y()));
            newPos.setX(m_dragStartItemPos.x() + delta.x());
            newPos.setY(m_dragStartItemPos.y() + delta.y());
            break;
        case 1: // Top-Right
            newSize.setWidth(qMax(20.0, m_dragStartSize.width() + delta.x()));
            newSize.setHeight(qMax(20.0, m_dragStartSize.height() - delta.y()));
            newPos.setY(m_dragStartItemPos.y() + delta.y());
            break;
        case 2: // Bottom-Left
            newSize.setWidth(qMax(20.0, m_dragStartSize.width() - delta.x()));
            newSize.setHeight(qMax(20.0, m_dragStartSize.height() + delta.y()));
            newPos.setX(m_dragStartItemPos.x() + delta.x());
            break;
        case 3: // Bottom-Right
            newSize.setWidth(qMax(20.0, m_dragStartSize.width() + delta.x()));
            newSize.setHeight(qMax(20.0, m_dragStartSize.height() + delta.y()));
            break;
    }

    newSize.setWidth(snapToGrid(newSize.width()));
    newSize.setHeight(snapToGrid(newSize.height()));
    newPos.setX(snapToGrid(newPos.x()));
    newPos.setY(snapToGrid(newPos.y()));

    setSize(newSize);
    setPos(newPos);
    event->accept();
}

void FloorPlanItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    m_draggedHandle = -1;
    QGraphicsObject::mouseReleaseEvent(event);
}

int FloorPlanItem::snapToGrid(qreal value) const {
    int gridSize = 20;
    if (scene()) {
        FloorPlanScene* fpScene = qobject_cast<FloorPlanScene*>(scene());
        if (fpScene) gridSize = fpScene->gridSize();
    }
    return qRound(value / gridSize) * gridSize;
}
