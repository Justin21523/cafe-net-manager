#pragma once

#include "models/MenuCategory.h"
#include "models/MenuItem.h"
#include <vector>

class MenuRepository;

class MenuService {
public:
    explicit MenuService(MenuRepository *repository);

    std::vector<MenuCategory> loadCategories();
    std::vector<MenuItem> loadItemsByCategory(int categoryId);

private:
    MenuRepository *m_repository;
};