// InventoryService.cpp
#include "services/InventoryService.h"
#include "database/DatabaseManager.h"
#include "utils/DatabaseTransaction.h"
#include "utils/Logger.h"
#include <QSqlQuery>
#include <QSqlError>

InventoryService::InventoryService(DatabaseManager *dbManager) : m_dbManager(dbManager) {}

bool InventoryService::deductStockForOrder(const Order &order) {
    DatabaseTransaction tx(m_dbManager->database());
    QSqlQuery query(m_dbManager->database());

    for (const auto &item : order.items) {
        // Find BOM for this menu item
        query.prepare("SELECT i.id, i.name, i.current_stock, mii.quantity "
                      "FROM menu_item_ingredients mii "
                      "JOIN ingredients i ON mii.ingredient_id = i.id "
                      "WHERE mii.menu_item_id = :menu_item_id");
        query.bindValue(":menu_item_id", item.menuItemId);
        
        if (!query.exec()) continue;

        while (query.next()) {
            int ingId = query.value("id").toInt();
            QString ingName = query.value("name").toString();
            double currentStock = query.value("current_stock").toDouble();
            double requiredQty = query.value("quantity").toDouble() * item.quantity;

            if (currentStock < requiredQty) {
                Logger::error("Insufficient stock for " + ingName + ". Required: " + 
                              QString::number(requiredQty) + ", Available: " + QString::number(currentStock));
                return false; // Transaction will auto-rollback
            }

            // Deduct stock
            query.prepare("UPDATE ingredients SET current_stock = current_stock - :qty WHERE id = :id");
            query.bindValue(":qty", requiredQty);
            query.bindValue(":id", ingId);
            query.exec();

            // Log movement
            query.prepare("INSERT INTO stock_movements (ingredient_id, quantity_change, movement_type, reference_id) "
                          "VALUES (:ing_id, :qty, 'DEDUCTION', :order_id)");
            query.bindValue(":ing_id", ingId);
            query.bindValue(":qty", -requiredQty);
            query.bindValue(":order_id", order.id);
            query.exec();
        }
    }

    return tx.commit();
}

double InventoryService::getIngredientStock(const QString &ingredientName) {
    QSqlQuery query(m_dbManager->database());
    query.prepare("SELECT current_stock FROM ingredients WHERE name = :name");
    query.bindValue(":name", ingredientName);
    if (query.exec() && query.next()) {
        return query.value("current_stock").toDouble();
    }
    return 0.0;
}