#include "widgets/FloorPlanView.h"
#include "graphics/FloorPlanScene.h"
#include "graphics/SeatItem.h"
#include "graphics/WallItem.h"
#include "graphics/CounterItem.h"
#include "graphics/LineItem.h"
#include "utils/Command.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>

FloorPlanView::FloorPlanView(FloorPlanScene *scene, QWidget *parent) 
    : QGraphicsView(scene, parent) {
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
}

void FloorPlanView::setEditMode(bool enabled) {
    m_isEditMode = enabled;
    // Update all items in scene
    for (QGraphicsItem* item : scene()->items()) {
        if (FloorPlanItem* fpItem = dynamic_cast<FloorPlanItem*>(item)) {
            fpItem->setEditMode(enabled);
        }
    }
}

void FloorPlanView::setCurrentTool(ObjectType type, ShapeType shape) {
    m_currentTool = type; m_currentShape = shape; m_isSelectMode = false;
    setDragMode(QGraphicsView::NoDrag);
}

void FloorPlanView::setSelectMode() { m_isSelectMode = true; setDragMode(QGraphicsView::RubberBandDrag); }

void FloorPlanView::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) { QGraphicsView::mousePressEvent(event); return; }

    QPointF scenePos = mapToScene(event->pos());
    FloorPlanScene *fpScene = qobject_cast<FloorPlanScene*>(scene());
    int gridSize = fpScene ? fpScene->gridSize() : 20;
    scenePos.setX(qRound(scenePos.x() / gridSize) * gridSize);
    scenePos.setY(qRound(scenePos.y() / gridSize) * gridSize);

    // 1. Line Tool Logic
    if (m_currentTool == ObjectType::LINE && m_isEditMode) {
        if (!m_isDrawingLine) {
            m_isDrawingLine = true;
            m_lineStartPos = scenePos;
            m_tempLine = new LineItem(scenePos, scenePos);
            fpScene->addItem(m_tempLine);
        }
        return;
    }
    
    // 2. Track Move/Resize for Undo
    if (m_isEditMode && !m_isSelectMode) {
        QGraphicsItem *itemAt = scene()->itemAt(scenePos, transform());
        m_draggedItem = dynamic_cast<FloorPlanItem*>(itemAt);
        if (m_draggedItem) {
            m_itemOldPos = m_draggedItem->pos();
            m_itemOldSize = m_draggedItem->size();
        }
    }

    // 3. Default Object Placement
    if (m_isSelectMode || m_currentTool == ObjectType::LINE) {
        QGraphicsView::mousePressEvent(event);
        return;
    }

    FloorPlanItem *newItem = nullptr;
    if (m_currentTool == ObjectType::SEAT) {
        Seat s; s.code = "NEW"; s.width = 60; s.height = 60;
        newItem = new SeatItem(s);
    } else if (m_currentTool == ObjectType::WALL) {
        newItem = new WallItem();
    } else if (m_currentTool == ObjectType::COUNTER) {
        newItem = new CounterItem();
    }

    if (newItem) {
        newItem->setShapeType(m_currentShape);
        newItem->setPos(scenePos);
        newItem->setEditMode(m_isEditMode);
        fpScene->addItem(newItem);
    }
}

void FloorPlanView::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDrawingLine && m_tempLine) {
        QPointF scenePos = mapToScene(event->pos());
        m_tempLine->updateLine(scenePos);
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void FloorPlanView::mouseReleaseEvent(QMouseEvent *event) {
    if (m_isDrawingLine && m_tempLine) {
        QPointF scenePos = mapToScene(event->pos());
        m_tempLine->updateLine(scenePos);
        m_isDrawingLine = false;
        m_tempLine = nullptr;
        return;
    }

    // Push Undo Commands
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
    // Undo / Redo
    if (event->matches(QKeySequence::Undo)) { m_undoStack.undo(); return; }
    if (event->matches(QKeySequence::Redo)) { m_undoStack.redo(); return; }

    // Delete
    if (event->key() == Qt::Key_Delete) {
        QList<QGraphicsItem*> selected = scene()->selectedItems();
        for (QGraphicsItem* item : selected) {
            scene()->removeItem(item);
            delete item;
        }
        return;
    }
    QGraphicsView::keyPressEvent(event);
}