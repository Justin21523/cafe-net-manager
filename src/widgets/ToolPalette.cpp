#include "widgets/ToolPalette.h"
#include <QVBoxLayout>
#include <QPushButton>

ToolPalette::ToolPalette(QWidget *parent) : QWidget(parent) { setupUI(); }

void ToolPalette::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);

    // Mode Toggle
    m_editModeBtn = new QPushButton("👁️ Preview Mode", this);
    m_editModeBtn->setCheckable(true);
    m_editModeBtn->setStyleSheet("background-color: #555; color: white; font-weight: bold;");
    connect(m_editModeBtn, &QPushButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_editModeBtn->setText("✏️ Edit Mode");
            m_editModeBtn->setStyleSheet("background-color: #ff9800; color: black; font-weight: bold;");
        } else {
            m_editModeBtn->setText("👁️ Preview Mode");
            m_editModeBtn->setStyleSheet("background-color: #555; color: white; font-weight: bold;");
        }
        emit editModeToggled(checked);
    });
    layout->addWidget(m_editModeBtn);
    layout->addSpacing(10);

    // Select Tool
    QPushButton *selectBtn = new QPushButton("🖱️ Select", this);
    selectBtn->setCheckable(true); selectBtn->setChecked(true);
    connect(selectBtn, &QPushButton::clicked, this, &ToolPalette::selectModeRequested);
    layout->addWidget(selectBtn);
    m_buttons.append(selectBtn);

    // Object Tools
    layout->addWidget(createToolButton("🪑 Seat (Rect)", ObjectType::SEAT, ShapeType::Rectangle));
    layout->addWidget(createToolButton("🟢 Seat (Circle)", ObjectType::SEAT, ShapeType::Circle));
    layout->addWidget(createToolButton("🧱 Wall", ObjectType::WALL, ShapeType::Rectangle));
    layout->addWidget(createToolButton("🛒 Counter", ObjectType::COUNTER, ShapeType::Rectangle));
    layout->addWidget(createToolButton("🔺 Decor (Tri)", ObjectType::DOOR, ShapeType::Triangle));
    layout->addWidget(createToolButton("🔺 Decor (Tri)", ObjectType::DOOR, ShapeType::Triangle));
    layout->addWidget(createToolButton("📏 Line", ObjectType::LINE, ShapeType::Rectangle)); // Add Line Tool
        
    layout->addStretch();
}

QPushButton* ToolPalette::createToolButton(const QString &text, ObjectType type, ShapeType shape) {
    QPushButton *btn = new QPushButton(text, this);
    btn->setCheckable(true);
    connect(btn, &QPushButton::clicked, this, [this, btn, type, shape]() {
        for (QPushButton* b : m_buttons) b->setChecked(false);
        btn->setChecked(true);
        emit toolSelected(type, shape);
    });
    m_buttons.append(btn);
    return btn;
}