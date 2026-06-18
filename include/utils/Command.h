#pragma once
#include <QUndoCommand>
#include <QPointF>
#include <QSizeF>

class FloorPlanItem;

// Move Command
class MoveCommand : public QUndoCommand {
public:
    MoveCommand(FloorPlanItem *item, const QPointF &oldPos, const QPointF &newPos);
    void undo() override;
    void redo() override;
private:
    FloorPlanItem *m_item;
    QPointF m_oldPos, m_newPos;
};

// Resize Command
class ResizeCommand : public QUndoCommand {
public:
    ResizeCommand(FloorPlanItem *item, const QSizeF &oldSize, const QSizeF &newSize);
    void undo() override;
    void redo() override;
private:
    FloorPlanItem *m_item;
    QSizeF m_oldSize, m_newSize;
};