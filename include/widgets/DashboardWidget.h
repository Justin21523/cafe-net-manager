#pragma once

#include <QWidget>
#include "database/DatabaseManager.h"

class QLabel;
class QTableWidget;
class DatabaseManager;
// 在 DashboardWidget.h 中加入指標：
class InventoryService;

class DashboardWidget : public QWidget {
    Q_OBJECT

public:
    explicit DashboardWidget(DatabaseManager *dbManager, InventoryService *invService, QWidget *parent = nullptr);

    void refreshData();

private:
    void setupUI();
    void loadTodayStats();
    void loadTopMenuItems();
    void loadHourlyRevenue();
    void loadInventoryStatus();

    DatabaseManager *m_dbManager;
    
    QLabel *m_revenueLabel;
    QLabel *m_ordersLabel;
    QLabel *m_activeSeatsLabel;
    QLabel *m_inventoryLabel;
    QTableWidget *m_topItemsTable;
    QTableWidget *m_hourlyRevenueTable;
    InventoryService *m_invService;
};