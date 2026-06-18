#pragma once
#include <QWidget>
#include "graphics/FloorPlanItem.h"
class QPushButton;

class ToolPalette : public QWidget {
    Q_OBJECT
public:
    explicit ToolPalette(QWidget *parent = nullptr);
signals:
    void toolSelected(ObjectType type, ShapeType shape);
    void selectModeRequested();
    void editModeToggled(bool isEditMode);
    void deleteRequested(); // NEW
private:
    void setupUI();
    QPushButton* createToolButton(const QString &text, ObjectType type, ShapeType shape);
    QList<QPushButton*> m_buttons;
    QPushButton *m_editModeBtn;
    QPushButton *m_deleteBtn;
};