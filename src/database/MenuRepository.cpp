#include "database/MenuRepository.h"
#include "database/DatabaseManager.h"
#include "utils/Logger.h"

#include <QSqlQuery>
#include <QSqlError>

MenuRepository::MenuRepository(DatabaseManager *dbManager)
    : m_dbManager(dbManager) {}

std::vector<MenuCategory> MenuRepository::getAllCategories() {
    std::vector<MenuCategory> categories;
    QSqlQuery query(m_dbManager->database());
    
    // Order by sort_order to ensure UI displays them correctly
    query.prepare("SELECT id, name, sort_order FROM menu_categories ORDER BY sort_order ASC");

    if (!query.exec()) {
        Logger::error("Failed to fetch categories: " + query.lastError().text());
        return categories;
    }

    while (query.next()) {
        MenuCategory cat;
        cat.id = query.value("id").toInt();
        cat.name = query.value("name").toString();
        cat.sortOrder = query.value("sort_order").toInt();
        categories.push_back(cat);
    }
    return categories;
}

std::vector<MenuItem> MenuRepository::getAllItems() {
    std::vector<MenuItem> items;
    QSqlQuery query(m_dbManager->database());

    query.prepare("SELECT id, category_id, name, description, price, is_available "
                  "FROM menu_items WHERE is_active = 1 ORDER BY category_id ASC, name ASC");

    if (!query.exec()) {
        Logger::error("Failed to fetch all menu items: " + query.lastError().text());
        return items;
    }

    while (query.next()) {
        MenuItem item;
        item.id = query.value("id").toInt();
        item.categoryId = query.value("category_id").toInt();
        item.name = query.value("name").toString();
        item.description = query.value("description").toString();
        item.price = query.value("price").toInt();
        item.isAvailable = query.value("is_available").toBool();
        items.push_back(item);
    }
    return items;
}

std::vector<MenuItem> MenuRepository::getItemsByCategoryId(int categoryId) {
    std::vector<MenuItem> items;
    QSqlQuery query(m_dbManager->database());
    
    query.prepare("SELECT id, category_id, name, description, price, is_available "
                  "FROM menu_items WHERE category_id = :cat_id AND is_active = 1 ORDER BY name ASC");
    query.bindValue(":cat_id", categoryId);

    if (!query.exec()) {
        Logger::error("Failed to fetch menu items: " + query.lastError().text());
        return items;
    }

    while (query.next()) {
        MenuItem item;
        item.id = query.value("id").toInt();
        item.categoryId = query.value("category_id").toInt();
        item.name = query.value("name").toString();
        item.description = query.value("description").toString();
        item.price = query.value("price").toInt();
        item.isAvailable = query.value("is_available").toBool();
        items.push_back(item);
    }
    return items;
}
