#pragma once
#include <QGraphicsView>
#include "graphics/FloorPlanItem.h"

class FloorPlanScene;

class FloorPlanView : public QGraphicsView {
    Q_OBJECT
public:
    explicit FloorPlanView(FloorPlanScene *scene, QWidget *parent = nullptr);

    void setCurrentTool(ObjectType type, ShapeType shape);
    void setSelectMode();
    void setEditMode(bool enabled);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override; // For shortcuts

private:
    ObjectType m_currentTool = ObjectType::SEAT;
    ShapeType m_currentShape = ShapeType::Rectangle;
    bool m_isSelectMode = true;
    bool m_isEditMode = false;
    
    // Clipboard for Copy/Paste
    struct ClipboardData { ObjectType type; ShapeType shape; QSizeF size; };
    QList<ClipboardData> m_clipboard;
};