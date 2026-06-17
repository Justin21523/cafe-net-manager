#pragma once

#include "models/MenuCategory.h"
#include "models/MenuItem.h"
#include <vector>

class DatabaseManager;

class MenuRepository {
public:
    explicit MenuRepository(DatabaseManager *dbManager);

    std::vector<MenuCategory> getAllCategories();
    std::vector<MenuItem> getItemsByCategoryId(int categoryId);

private:
    DatabaseManager *m_dbManager;
};