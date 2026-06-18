#pragma once
#include <QGraphicsView>
#include <QUndoStack>
#include "graphics/FloorPlanItem.h"

class FloorPlanScene;
class LineItem;

class FloorPlanView : public QGraphicsView {
    Q_OBJECT
public:
    explicit FloorPlanView(FloorPlanScene *scene, QWidget *parent = nullptr);
    void setCurrentTool(ObjectType type, ShapeType shape);
    void setSelectMode();
    void setEditMode(bool enabled);
    bool isEditMode() const { return m_isEditMode; }
    QUndoStack* undoStack() { return &m_undoStack; }

signals:
    void seatClickedInPreview(int seatId); // For popup

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    ObjectType m_currentTool = ObjectType::SEAT;
    ShapeType m_currentShape = ShapeType::Rectangle;
    bool m_isSelectMode = true;
    bool m_isEditMode = false;
    QUndoStack m_undoStack;

    // Line drawing
    bool m_isDrawingLine = false;
    QPointF m_lineStartPos;
    LineItem *m_tempLine = nullptr;
};