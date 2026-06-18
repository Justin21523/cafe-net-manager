// InventoryService.h
#pragma once
#include <vector>
#include "models/Order.h"
class DatabaseManager;

class InventoryService {
public:
    explicit InventoryService(DatabaseManager *dbManager);
    
    // Deducts ingredients based on Order's BOM. Returns false if stock is insufficient.
    bool deductStockForOrder(const Order &order);
    double getIngredientStock(const QString &ingredientName);

private:
    DatabaseManager *m_dbManager;
};
