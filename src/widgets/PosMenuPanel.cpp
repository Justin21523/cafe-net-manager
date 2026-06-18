#include "widgets/PosMenuPanel.h"
#include "widgets/ModifierDialog.h"
#include "services/MenuService.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>

PosMenuPanel::PosMenuPanel(MenuService *service, QWidget *parent)
    : QWidget(parent), m_service(service) {
    setupUI();
    loadMenuData();
}

void PosMenuPanel::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Search Bar
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("🔍 Search menu items...");
    m_searchEdit->setStyleSheet("padding: 8px; font-size: 14px; border: 1px solid #ccc; border-radius: 4px;");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &PosMenuPanel::onSearchTextChanged);
    mainLayout->addWidget(m_searchEdit);

    QHBoxLayout *contentLayout = new QHBoxLayout();

    // Left: Categories
    m_categoryList = new QListWidget(this);
    m_categoryList->setFixedWidth(150);
    m_categoryList->setStyleSheet("QListWidget::item { padding: 10px; font-size: 14px; } QListWidget::item:selected { background-color: #4CAF50; color: white; }");
    connect(m_categoryList, &QListWidget::currentRowChanged, this, &PosMenuPanel::onCategoryChanged);
    contentLayout->addWidget(m_categoryList);

    // Right: Menu Grid
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget *gridContainer = new QWidget();
    m_gridLayout = new QGridLayout(gridContainer);
    m_gridLayout->setSpacing(10);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    
    m_scrollArea->setWidget(gridContainer);
    contentLayout->addWidget(m_scrollArea, 1);

    mainLayout->addLayout(contentLayout);
}

void PosMenuPanel::loadMenuData() {
    m_categories = m_service->loadCategories();
    m_allItems = m_service->loadAllItems(); // Note: Need to add this to MenuService

    // Populate Categories
    m_categoryList->addItem("All Categories");
    for (const auto &cat : m_categories) {
        m_categoryList->addItem(cat.name);
    }
    m_categoryList->setCurrentRow(0);

    // Create Cards
    int row = 0, col = 0;
    for (const auto &item : m_allItems) {
        QPushButton *card = createMenuCard(item);
        m_gridLayout->addWidget(card, row, col);
        
        m_cards.push_back({card, item.categoryId, item.name});
        
        col++;
        if (col >= 3) { // 3 columns
            col = 0;
            row++;
        }
    }
}

QPushButton* PosMenuPanel::createMenuCard(const MenuItem &item) {
    QPushButton *card = new QPushButton(this);
    card->setFixedSize(180, 120);
    card->setCursor(Qt::PointingHandCursor);
    
    QString priceStr = QString("$%1").arg(item.price / 100.0, 0, 'f', 2);
    card->setText(QString("<b>%1</b><br><span style='color:green; font-size:16px;'>%2</span>")
                  .arg(item.name).arg(priceStr));
    
    card->setStyleSheet(R"(
        QPushButton {
            background-color: #f9f9f9;
            border: 1px solid #ddd;
            border-radius: 8px;
            text-align: center;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #e0f2f1;
            border: 1px solid #4CAF50;
        }
    )");

    connect(card, &QPushButton::clicked, this, [this, item]() {
        onCardClicked(item.id);
    });

    return card;
}

void PosMenuPanel::onCardClicked(int menuItemId) {
    // Find item
    MenuItem targetItem;
    for (const auto &item : m_allItems) {
        if (item.id == menuItemId) {
            targetItem = item;
            break;
        }
    }

    ModifierDialog dialog(targetItem, this);
    if (dialog.exec() == QDialog::Accepted) {
        emit itemAddRequested(targetItem, dialog.getModifiers());
    }
}

void PosMenuPanel::onCategoryChanged(int row) {
    if (row == 0) {
        m_currentCategoryId = -1; // All
    } else {
        m_currentCategoryId = m_categories[row - 1].id;
    }
    applyFilters();
}

void PosMenuPanel::onSearchTextChanged(const QString &text) {
    m_currentSearchText = text.trimmed().toLower();
    applyFilters();
}

void PosMenuPanel::applyFilters() {
    for (const auto &data : m_cards) {
        bool matchCat = (m_currentCategoryId == -1) || (data.categoryId == m_currentCategoryId);
        bool matchSearch = m_currentSearchText.isEmpty() || data.name.toLower().contains(m_currentSearchText);
        
        data.card->setVisible(matchCat && matchSearch);
    }
}