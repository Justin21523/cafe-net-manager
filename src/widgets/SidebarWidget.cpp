#include "widgets/SidebarWidget.h"
#include <QVBoxLayout>
#include <QLabel>

SidebarWidget::SidebarWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void SidebarWidget::setupUI() {
    setFixedWidth(200);
    setStyleSheet("background-color: #2b2b2b; border-right: 1px solid #444;");
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 20, 10, 20);
    layout->setSpacing(10);

    // App Title
    QLabel *titleLabel = new QLabel("CafeNet", this);
    titleLabel->setStyleSheet("color: white; font-size: 22px; font-weight: bold; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    layout->addSpacing(20);

    // Navigation Buttons
    m_buttons.append(createNavButton("📊 Dashboard", 0));
    m_buttons.append(createNavButton("🪑 Floor Plan", 1));
    m_buttons.append(createNavButton("🛒 POS / Orders", 2));
    m_buttons.append(createNavButton("👨‍🍳 Kitchen Board", 3));
    m_buttons.append(createNavButton("⚙️ Admin", 4));
    
    for (QPushButton* btn : m_buttons) {
        layout->addWidget(btn);
    }
    
    layout->addStretch();
    
    // Default selection
    if (!m_buttons.isEmpty()) {
        m_buttons[0]->setChecked(true);
    }
}

QPushButton* SidebarWidget::createNavButton(const QString &text, int index) {
    QPushButton *btn = new QPushButton(text, this);
    btn->setCheckable(true);
    btn->setCursor(Qt::PointingHandCursor);
    
    btn->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            color: #cccccc;
            border: none;
            padding: 12px 15px;
            text-align: left;
            border-radius: 6px;
            font-size: 15px;
        }
        QPushButton:hover {
            background-color: #2d2d2d;
            color: #ffffff;
        }
        QPushButton:checked {
            background-color: #4CAF50;
            color: #ffffff;
            font-weight: bold;
        }
    )");
    
    connect(btn, &QPushButton::clicked, this, [this, index, btn]() {
        for (QPushButton* b : m_buttons) b->setChecked(false);
        btn->setChecked(true);
        emit navigateTo(index);
    });

    return btn;
}
