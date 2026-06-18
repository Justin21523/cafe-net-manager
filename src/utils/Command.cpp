#include "utils/Command.h"
#include "graphics/FloorPlanItem.h"

MoveCommand::MoveCommand(FloorPlanItem *item, const QPointF &oldPos, const QPointF &newPos)
    : m_item(item), m_oldPos(oldPos), m_newPos(newPos) {
    setText("Move");
}
void MoveCommand::undo() { m_item->setPos(m_oldPos); }
void MoveCommand::redo() { m_item->setPos(m_newPos); }

ResizeCommand::ResizeCommand(FloorPlanItem *item, const QSizeF &oldSize, const QSizeF &newSize)
    : m_item(item), m_oldSize(oldSize), m_newSize(newSize) {
    setText("Resize");
}
void ResizeCommand::undo() { m_item->setSize(m_oldSize); }
void ResizeCommand::redo() { m_item->setSize(m_newSize); }