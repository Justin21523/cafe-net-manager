#include "widgets/MenuBrowserWidget.h"
#include "widgets/MenuItemCard.h"
#include "services/MenuService.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>

MenuBrowserWidget::MenuBrowserWidget(MenuService *service, QWidget *parent)
    : QWidget(parent), m_service(service) {
    setupUI();
    if (m_service) {
        loadCategories();
    }
}

void MenuBrowserWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Category Buttons Area
    m_categoryLayout = new QHBoxLayout();
    m_categoryLayout->setSpacing(5);
    mainLayout->addLayout(m_categoryLayout);

    // Items Scroll Area
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    
    m_itemsContainer = new QWidget();
    m_itemsLayout = new QVBoxLayout(m_itemsContainer);
    m_itemsLayout->setSpacing(10);
    m_itemsLayout->addStretch(); // Push items to the top

    m_scrollArea->setWidget(m_itemsContainer);
    mainLayout->addWidget(m_scrollArea);
}

void MenuBrowserWidget::loadCategories() {
    // Clear existing buttons
    for (QPushButton *btn : m_categoryButtons) {
        delete btn;
    }
    m_categoryButtons.clear();

    std::vector<MenuCategory> categories = m_service->loadCategories();
    
    for (const auto &cat : categories) {
        QPushButton *btn = new QPushButton(cat.name, this);
        btn->setCheckable(true);
        connect(btn, &QPushButton::clicked, this, [this, cat]() {
            // Reset all buttons style
            for (QPushButton *b : m_categoryButtons) b->setChecked(false);
            // Set current button style
            // (Simplified: just rely on checkable state for now)
            loadItemsForCategory(cat.id);
        });
        m_categoryLayout->addWidget(btn);
        m_categoryButtons.push_back(btn);
    }
    
    // Auto select first category if exists
    if (!m_categoryButtons.empty()) {
        m_categoryButtons.front()->click(); 
    }
}

void MenuBrowserWidget::loadItemsForCategory(int categoryId) {
    clearItemCards();

    std::vector<MenuItem> items = m_service->loadItemsByCategory(categoryId);
    
    // Calculate grid layout: 3 items per row
    QHBoxLayout *currentRowLayout = nullptr;
    int itemsInRow = 0;

    for (const auto &item : items) {
        if (itemsInRow % 3 == 0) {
            currentRowLayout = new QHBoxLayout();
            currentRowLayout->setSpacing(5); // Add some spacing between cards
            // 插入到最後一個位置（在 stretch 之前）
            m_itemsLayout->insertLayout(m_itemsLayout->count() - 1, currentRowLayout);
        }

        MenuItemCard *card = new MenuItemCard(item, m_itemsContainer);
        connect(card, &MenuItemCard::addToCartRequested, this, &MenuBrowserWidget::itemAddedToCart);
        
        currentRowLayout->addWidget(card);
        itemsInRow++;
    }
}

void MenuBrowserWidget::clearItemCards() {
    // 只刪除 MenuItemCard widgets，不刪除 row layouts
    QList<MenuItemCard*> cards = m_itemsContainer->findChildren<MenuItemCard*>();
    for (MenuItemCard *card : cards) {
        delete card;
    }
    
    // 刪除所有 QHBoxLayout（row layouts）
    QList<QHBoxLayout*> rowLayouts = m_itemsContainer->findChildren<QHBoxLayout*>();
    for (QHBoxLayout *layout : rowLayouts) {
        delete layout;
    }
}