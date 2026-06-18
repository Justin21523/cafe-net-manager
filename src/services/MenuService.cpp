#include "services/MenuService.h"
#include "database/MenuRepository.h"

MenuService::MenuService(MenuRepository *repository)
    : m_repository(repository) {}

std::vector<MenuCategory> MenuService::loadCategories() {
    return m_repository->getAllCategories();
}

std::vector<MenuItem> MenuService::loadAllItems() {
    return m_repository->getAllItems();
}

std::vector<MenuItem> MenuService::loadItemsByCategory(int categoryId) {
    return m_repository->getItemsByCategoryId(categoryId);
}
