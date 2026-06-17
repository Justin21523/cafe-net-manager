#include "widgets/FloorPlanView.h"
#include "graphics/FloorPlanScene.h"
#include "graphics/SeatItem.h"
#include "graphics/WallItem.h"
#include "graphics/CounterItem.h"
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
    if (m_isSelectMode || event->button() != Qt::LeftButton) {
        QGraphicsView::mousePressEvent(event); return;
    }

    QPointF scenePos = mapToScene(event->pos());
    FloorPlanScene *fpScene = qobject_cast<FloorPlanScene*>(scene());
    int gridSize = fpScene ? fpScene->gridSize() : 20;
    scenePos.setX(qRound(scenePos.x() / gridSize) * gridSize);
    scenePos.setY(qRound(scenePos.y() / gridSize) * gridSize);

    // Create item based on tool (Simplified: using generic FloorPlanItem logic via derived classes)
    // For brevity, we'll instantiate WallItem/CounterItem and set their shape/size
    FloorPlanItem *newItem = nullptr;
    if (m_currentTool == ObjectType::SEAT) {
        Seat s; s.code = "NEW"; s.width = 60; s.height = 60;
        newItem = new SeatItem(s);
    } else if (m_currentTool == ObjectType::WALL) {
        newItem = new WallItem();
    } else if (m_currentTool == ObjectType::COUNTER) {
        newItem = new CounterItem();
    } else {
        newItem = new WallItem(); // Fallback for Triangle/Door
    }

    if (newItem) {
        newItem->setShapeType(m_currentShape);
        newItem->setPos(scenePos);
        newItem->setEditMode(m_isEditMode);
        fpScene->addItem(newItem);
    }
}

void FloorPlanView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        QList<QGraphicsItem*> selected = scene()->selectedItems();
        for (QGraphicsItem* item : selected) {
            if (FloorPlanItem* fpItem = dynamic_cast<FloorPlanItem*>(item)) {
                scene()->removeItem(fpItem);
                delete fpItem;
            }
        }
    } 
    else if (event->matches(QKeySequence::Copy)) {
        m_clipboard.clear();
        for (QGraphicsItem* item : scene()->selectedItems()) {
            if (FloorPlanItem* fpItem = dynamic_cast<FloorPlanItem*>(item)) {
                m_clipboard.append({fpItem->objectType(), fpItem->shapeType(), fpItem->size()});
            }
        }
    } 
    else if (event->matches(QKeySequence::Paste)) {
        for (const auto& data : m_clipboard) {
            FloorPlanItem* newItem = nullptr;
            if (data.type == ObjectType::SEAT) { Seat s; s.width=data.size.width(); s.height=data.size.height(); newItem = new SeatItem(s); }
            else if (data.type == ObjectType::WALL) { newItem = new WallItem(); }
            else { newItem = new CounterItem(); }
            
            newItem->setShapeType(data.shape);
            newItem->setSize(data.size);
            newItem->setPos(mapToScene(viewport()->rect().center())); // Paste in center
            newItem->setEditMode(m_isEditMode);
            scene()->addItem(newItem);
        }
    }
    else {
        QGraphicsView::keyPressEvent(event);
    }
}