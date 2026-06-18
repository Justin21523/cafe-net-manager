#include "widgets/ToolPalette.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

ToolPalette::ToolPalette(QWidget *parent) : QWidget(parent) { setupUI(); }

void ToolPalette::setupUI() {
    setFixedWidth(180);
    setStyleSheet("background-color: #1e1e1e; border-right: 1px solid #333;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(6);
    layout->setContentsMargins(10, 10, 10, 10);

    // Mode Toggle
    m_editModeBtn = new QPushButton("👁️ Preview", this);
    m_editModeBtn->setCheckable(true);
    m_editModeBtn->setMinimumHeight(40);
    m_editModeBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #555555;
            color: #ffffff;
            font-weight: bold;
            border-radius: 4px;
            font-size: 13px;
            border: none;
        }
        QPushButton:checked {
            background-color: #ff9800;
            color: #000000;
        }
    )");
    connect(m_editModeBtn, &QPushButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_editModeBtn->setText("✏️ Edit Mode");
        } else {
            m_editModeBtn->setText("️ Preview");
        }
        emit editModeToggled(checked);
    });
    layout->addWidget(m_editModeBtn);
    layout->addSpacing(8);

    // Select Tool
    QPushButton *selectBtn = new QPushButton("🖱️ Select", this);
    selectBtn->setCheckable(true);
    selectBtn->setChecked(true);
    selectBtn->setMinimumHeight(36);
    selectBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #333333;
            color: #ffffff;
            text-align: left;
            padding: 6px 10px;
            font-size: 13px;
            border-radius: 4px;
            border: none;
        }
        QPushButton:checked {
            background-color: #4CAF50;
            color: #ffffff;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #444444;
        }
    )");
    connect(selectBtn, &QPushButton::clicked, this, &ToolPalette::selectModeRequested);
    layout->addWidget(selectBtn);
    m_buttons.append(selectBtn);

    // Delete Button
    m_deleteBtn = new QPushButton("🗑️ Delete Selected", this);
    m_deleteBtn->setMinimumHeight(36);
    m_deleteBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #f44336;
            color: #ffffff;
            font-weight: bold;
            border-radius: 4px;
            font-size: 13px;
            border: none;
        }
        QPushButton:hover {
            background-color: #d32f2f;
        }
    )");
    connect(m_deleteBtn, &QPushButton::clicked, this, &ToolPalette::deleteRequested);
    layout->addWidget(m_deleteBtn);

    layout->addSpacing(8);

    // Separator
    QLabel *sep = new QLabel("─ Place Objects ─", this);
    sep->setAlignment(Qt::AlignCenter);
    sep->setStyleSheet("color: #888888; font-size: 11px;");
    layout->addWidget(sep);

    // Object Tools
    layout->addWidget(createToolButton("🪑 Seat (Rect)", ObjectType::SEAT, ShapeType::Rectangle));
    layout->addWidget(createToolButton("🟢 Seat (Circle)", ObjectType::SEAT, ShapeType::Circle));
    layout->addWidget(createToolButton("🧱 Wall", ObjectType::WALL, ShapeType::Rectangle));
    layout->addWidget(createToolButton("🛒 Counter", ObjectType::COUNTER, ShapeType::Rectangle));
    layout->addWidget(createToolButton("🚪 Door", ObjectType::DOOR, ShapeType::Rectangle));
    layout->addWidget(createToolButton("📏 Line", ObjectType::LINE, ShapeType::Rectangle));

    layout->addStretch();

    // Shortcut hints
    QLabel *hints = new QLabel("Shortcuts:\nDel = Delete\nEsc = Deselect\nCtrl+Z = Undo", this);
    hints->setStyleSheet("color: #aaaaaa; font-size: 10px; background-color: #2a2a2a; padding: 8px; border-radius: 4px;");
    hints->setWordWrap(true);
    layout->addWidget(hints);
}

QPushButton* ToolPalette::createToolButton(const QString &text, ObjectType type, ShapeType shape) {
    QPushButton *btn = new QPushButton(text, this);
    btn->setCheckable(true);
    btn->setMinimumHeight(36);
    btn->setStyleSheet(R"(
        QPushButton {
            background-color: #333333;
            color: #ffffff;
            text-align: left;
            padding: 6px 10px;
            font-size: 13px;
            border-radius: 4px;
            border: none;
        }
        QPushButton:checked {
            background-color: #4CAF50;
            color: #ffffff;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #444444;
        }
    )");
    connect(btn, &QPushButton::clicked, this, [this, btn, type, shape]() {
        for (QPushButton* b : m_buttons) b->setChecked(false);
        btn->setChecked(true);
        emit toolSelected(type, shape);
    });
    m_buttons.append(btn);
    return btn;
}