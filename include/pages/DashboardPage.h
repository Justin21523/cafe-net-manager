#pragma once
#include <QWidget>

class DashboardWidget;
class DatabaseManager;

class DashboardPage : public QWidget {
    Q_OBJECT
public:
    explicit DashboardPage(DatabaseManager* dbManager, QWidget *parent = nullptr);
    void refreshData();

private:
    DashboardWidget* m_dashboardWidget;
};