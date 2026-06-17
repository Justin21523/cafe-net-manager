#pragma once

#include <QWidget>
#include <vector>
#include <QPushButton>
#include "models/MenuItem.h"

class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
class MenuService;

class MenuBrowserWidget : public QWidget {
    Q_OBJECT

public:
    explicit MenuBrowserWidget(MenuService *service, QWidget *parent = nullptr);

    void loadCategories();
    void loadItemsForCategory(int categoryId);

signals:
    void itemAddedToCart(const MenuItem &item);

private:
    void setupUI();
    void clearItemCards();

    MenuService *m_service;
    QHBoxLayout *m_categoryLayout;
    QVBoxLayout *m_itemsLayout;
    QWidget *m_itemsContainer;
    QScrollArea *m_scrollArea;
    
    std::vector<QPushButton*> m_categoryButtons; // Keep track to manage styles
};