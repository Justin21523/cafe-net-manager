#pragma once

#include <QWidget>

class QLabel;
class QTableWidget;
class DatabaseManager;

class DashboardWidget : public QWidget {
    Q_OBJECT

public:
    explicit DashboardWidget(DatabaseManager *dbManager, QWidget *parent = nullptr);

    void refreshData();

private:
    void setupUI();
    void loadTodayStats();
    void loadTopMenuItems();
    void loadHourlyRevenue();

    DatabaseManager *m_dbManager;
    
    QLabel *m_revenueLabel;
    QLabel *m_ordersLabel;
    QLabel *m_activeSeatsLabel;
    QTableWidget *m_topItemsTable;
    QTableWidget *m_hourlyRevenueTable;
};