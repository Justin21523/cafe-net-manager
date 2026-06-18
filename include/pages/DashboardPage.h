#pragma once
#include <QWidget>

class DashboardWidget;
class DatabaseManager;
class InventoryService;

class DashboardPage : public QWidget {
    Q_OBJECT
public:
    explicit DashboardPage(DatabaseManager* dbManager, InventoryService *invService = nullptr, QWidget *parent = nullptr);
    void refreshData();

private:
    DashboardWidget* m_dashboardWidget;
};
