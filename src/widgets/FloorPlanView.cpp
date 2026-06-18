#include "widgets/FloorPlanView.h"
#include "graphics/FloorPlanScene.h"
#include "graphics/SeatItem.h"
#include "graphics/EnvironmentItem.h"
#include "graphics/LineItem.h"
#include "utils/Command.h"

#include <QKeyEvent>
#include <QMouseEvent>

FloorPlanView::FloorPlanView(FloorPlanScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent) {
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
    setFocusPolicy(Qt::StrongFocus);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void FloorPlanView::setEditMode(bool enabled) {
    m_isEditMode = enabled;
    for (QGraphicsItem *item : scene()->items()) {
        if (FloorPlanItem *fpItem = dynamic_cast<FloorPlanItem*>(item)) {
            fpItem->setEditMode(enabled);
        }
        if (LineItem *line = dynamic_cast<LineItem*>(item)) {
            line->setFlag(QGraphicsItem::ItemIsMovable, enabled);
            line->setFlag(QGraphicsItem::ItemIsSelectable, enabled);
        }
    }

    if (!enabled) {
        scene()->clearSelection();
        setDragMode(QGraphicsView::NoDrag);
    } else {
        setDragMode(QGraphicsView::RubberBandDrag);
    }
}

void FloorPlanView::setCurrentTool(ObjectType type, ShapeType shape) {
    m_currentTool = type;
    m_currentShape = shape;
    m_isSelectMode = false;
    setDragMode(QGraphicsView::NoDrag);
    setCursor(Qt::CrossCursor);
}

void FloorPlanView::setSelectMode() {
    m_isSelectMode = true;
    if (m_isEditMode) {
        setDragMode(QGraphicsView::RubberBandDrag);
    }
    setCursor(Qt::ArrowCursor);
}

void FloorPlanView::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        QGraphicsView::mousePressEvent(event);
        return;
    }

    QPointF scenePos = mapToScene(event->pos());
    FloorPlanScene *fpScene = qobject_cast<FloorPlanScene*>(scene());
    int gridSize = fpScene ? fpScene->gridSize() : 20;
    QPointF snappedPos(qRound(scenePos.x() / gridSize) * gridSize,
                       qRound(scenePos.y() / gridSize) * gridSize);

    if (m_currentTool == ObjectType::LINE && m_isEditMode) {
        if (!m_isDrawingLine) {
            m_isDrawingLine = true;
            m_lineStartPos = snappedPos;
            m_tempLine = new LineItem(snappedPos, snappedPos);
            fpScene->addItem(m_tempLine);
        }
        event->accept();
        return;
    }

    if (!m_isEditMode) {
        QGraphicsItem *itemAt = scene()->itemAt(scenePos, transform());
        SeatItem *seatItem = dynamic_cast<SeatItem*>(itemAt);
        if (!seatItem && itemAt) {
            seatItem = dynamic_cast<SeatItem*>(itemAt->parentItem());
        }
        if (seatItem) {
            emit seatClickedInPreview(seatItem->seatData().id);
            event->accept();
            return;
        }
        QGraphicsView::mousePressEvent(event);
        return;
    }

    if (!m_isSelectMode && m_currentTool != ObjectType::LINE) {
        FloorPlanItem *newItem = nullptr;
        if (m_currentTool == ObjectType::SEAT) {
            Seat seat;
            seat.code = "NEW";
            seat.width = 60;
            seat.height = 60;
            newItem = new SeatItem(seat);
        } else {
            newItem = new EnvironmentItem(m_currentTool);
        }

        if (newItem) {
            newItem->setShapeType(m_currentShape);
            newItem->setPos(snappedPos);
            newItem->setEditMode(true);
            fpScene->addItem(newItem);
        }
        event->accept();
        return;
    }

    QGraphicsView::mousePressEvent(event);
}

void FloorPlanView::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDrawingLine && m_tempLine) {
        m_tempLine->updateEndPoint(mapToScene(event->pos()));
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void FloorPlanView::mouseReleaseEvent(QMouseEvent *event) {
    if (m_isDrawingLine && m_tempLine) {
        m_tempLine->updateEndPoint(mapToScene(event->pos()));
        m_isDrawingLine = false;
        m_tempLine = nullptr;
        event->accept();
        return;
    }

    if (m_draggedItem) {
        if (m_draggedItem->pos() != m_itemOldPos) {
            m_undoStack.push(new MoveCommand(m_draggedItem, m_itemOldPos, m_draggedItem->pos()));
        }
        if (m_draggedItem->size() != m_itemOldSize) {
            m_undoStack.push(new ResizeCommand(m_draggedItem, m_itemOldSize, m_draggedItem->size()));
        }
        m_draggedItem = nullptr;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void FloorPlanView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        if (m_isEditMode) {
            QList<QGraphicsItem*> selected = scene()->selectedItems();
            for (QGraphicsItem *item : selected) {
                scene()->removeItem(item);
                delete item;
            }
        }
        return;
    }

    if (event->matches(QKeySequence::Undo)) { m_undoStack.undo(); return; }
    if (event->matches(QKeySequence::Redo)) { m_undoStack.redo(); return; }
    if (event->matches(QKeySequence::SelectAll)) {
        if (m_isEditMode) {
            for (QGraphicsItem *item : scene()->items()) item->setSelected(true);
        }
        return;
    }
    if (event->key() == Qt::Key_Escape) {
        scene()->clearSelection();
        setSelectMode();
        return;
    }

    QGraphicsView::keyPressEvent(event);
}
