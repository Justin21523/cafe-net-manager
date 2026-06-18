#pragma once
#include <QWidget>
#include <vector>
#include "models/MenuCategory.h"
#include "models/MenuItem.h"

class QLineEdit;
class QListWidget;
class QGridLayout;
class QScrollArea;
class QPushButton;
class MenuService;

struct MenuCardData {
    QPushButton *card;
    int categoryId;
    QString name;
};

class PosMenuPanel : public QWidget {
    Q_OBJECT
public:
    explicit PosMenuPanel(MenuService *service, QWidget *parent = nullptr);

signals:
    void itemAddRequested(const MenuItem &item, const QString &note);

private slots:
    void onCategoryChanged(int row);
    void onSearchTextChanged(const QString &text);
    void onCardClicked(int menuItemId);

private:
    void setupUI();
    void loadMenuData();
    void applyFilters();
    QPushButton* createMenuCard(const MenuItem &item);

    MenuService *m_service;
    QLineEdit *m_searchEdit;
    QListWidget *m_categoryList;
    QGridLayout *m_gridLayout;
    QScrollArea *m_scrollArea;
    
    std::vector<MenuCategory> m_categories;
    std::vector<MenuItem> m_allItems;
    std::vector<MenuCardData> m_cards;
    
    int m_currentCategoryId = -1;
    QString m_currentSearchText = "";
};