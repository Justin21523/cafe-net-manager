#include "widgets/DashboardWidget.h"
#include "database/DatabaseManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QSqlQuery>
#include <QDateTime>
#include <QDate>

DashboardWidget::DashboardWidget(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager) {
    setupUI();
    refreshData();
}

void DashboardWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Dashboard - Today's Overview", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Stats Cards
    QGridLayout *statsLayout = new QGridLayout();
    
    QGroupBox *revenueBox = new QGroupBox("Today's Revenue", this);
    QVBoxLayout *revenueLayout = new QVBoxLayout(revenueBox);
    m_revenueLabel = new QLabel("$0.00", this);
    QFont bigFont = m_revenueLabel->font();
    bigFont.setPointSize(20);
    bigFont.setBold(true);
    m_revenueLabel->setFont(bigFont);
    m_revenueLabel->setAlignment(Qt::AlignCenter);
    m_revenueLabel->setStyleSheet("color: green;");
    revenueLayout->addWidget(m_revenueLabel);
    statsLayout->addWidget(revenueBox, 0, 0);

    QGroupBox *ordersBox = new QGroupBox("Total Orders", this);
    QVBoxLayout *ordersLayout = new QVBoxLayout(ordersBox);
    m_ordersLabel = new QLabel("0", this);
    m_ordersLabel->setFont(bigFont);
    m_ordersLabel->setAlignment(Qt::AlignCenter);
    m_ordersLabel->setStyleSheet("color: blue;");
    ordersLayout->addWidget(m_ordersLabel);
    statsLayout->addWidget(ordersBox, 0, 1);

    QGroupBox *seatsBox = new QGroupBox("Active Seats", this);
    QVBoxLayout *seatsLayout = new QVBoxLayout(seatsBox);
    m_activeSeatsLabel = new QLabel("0", this);
    m_activeSeatsLabel->setFont(bigFont);
    m_activeSeatsLabel->setAlignment(Qt::AlignCenter);
    m_activeSeatsLabel->setStyleSheet("color: orange;");
    seatsLayout->addWidget(m_activeSeatsLabel);
    statsLayout->addWidget(seatsBox, 0, 2);

    mainLayout->addLayout(statsLayout);

    // Top Menu Items
    QGroupBox *topItemsBox = new QGroupBox("Top 5 Menu Items", this);
    QVBoxLayout *topItemsLayout = new QVBoxLayout(topItemsBox);
    m_topItemsTable = new QTableWidget(this);
    m_topItemsTable->setColumnCount(3);
    m_topItemsTable->setHorizontalHeaderLabels({"Item Name", "Quantity Sold", "Revenue"});
    m_topItemsTable->horizontalHeader()->setStretchLastSection(true);
    m_topItemsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    topItemsLayout->addWidget(m_topItemsTable);
    mainLayout->addWidget(topItemsBox);

    // Hourly Revenue
    QGroupBox *hourlyBox = new QGroupBox("Hourly Revenue", this);
    QVBoxLayout *hourlyLayout = new QVBoxLayout(hourlyBox);
    m_hourlyRevenueTable = new QTableWidget(this);
    m_hourlyRevenueTable->setColumnCount(2);
    m_hourlyRevenueTable->setHorizontalHeaderLabels({"Hour", "Revenue"});
    m_hourlyRevenueTable->horizontalHeader()->setStretchLastSection(true);
    m_hourlyRevenueTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    hourlyLayout->addWidget(m_hourlyRevenueTable);
    mainLayout->addWidget(hourlyBox);
}

void DashboardWidget::refreshData() {
    loadTodayStats();
    loadTopMenuItems();
    loadHourlyRevenue();
}

void DashboardWidget::loadTodayStats() {
    QSqlQuery query(m_dbManager->database());
    
    // Today's revenue and order count
    query.prepare("SELECT COUNT(*) as order_count, SUM(total) as total_revenue "
                  "FROM orders WHERE DATE(created_at) = DATE('now') AND status = 'Paid'");
    
    if (query.exec() && query.next()) {
        int orderCount = query.value("order_count").toInt();
        int totalRevenue = query.value("total_revenue").toInt();
        
        m_ordersLabel->setText(QString::number(orderCount));
        m_revenueLabel->setText(QString("$%1").arg(totalRevenue / 100.0, 0, 'f', 2));
    }

    // Active seats
    query.prepare("SELECT COUNT(*) FROM seats WHERE status = 'Occupied'");
    if (query.exec() && query.next()) {
        int activeSeats = query.value(0).toInt();
        m_activeSeatsLabel->setText(QString::number(activeSeats));
    }
}

void DashboardWidget::loadTopMenuItems() {
    m_topItemsTable->setRowCount(0);
    
    QSqlQuery query(m_dbManager->database());
    query.prepare("SELECT oi.item_name, SUM(oi.quantity) as total_qty, SUM(oi.subtotal) as total_revenue "
                  "FROM order_items oi "
                  "JOIN orders o ON oi.order_id = o.id "
                  "WHERE DATE(o.created_at) = DATE('now') AND o.status = 'Paid' "
                  "GROUP BY oi.item_name "
                  "ORDER BY total_qty DESC "
                  "LIMIT 5");

    if (query.exec()) {
        while (query.next()) {
            int row = m_topItemsTable->rowCount();
            m_topItemsTable->insertRow(row);
            
            m_topItemsTable->setItem(row, 0, new QTableWidgetItem(query.value("item_name").toString()));
            m_topItemsTable->setItem(row, 1, new QTableWidgetItem(query.value("total_qty").toString()));
            
            double revenue = query.value("total_revenue").toInt() / 100.0;
            m_topItemsTable->setItem(row, 2, new QTableWidgetItem(QString("$%1").arg(revenue, 0, 'f', 2)));
        }
    }
}

void DashboardWidget::loadHourlyRevenue() {
    m_hourlyRevenueTable->setRowCount(0);
    
    QSqlQuery query(m_dbManager->database());
    query.prepare("SELECT strftime('%H', created_at) as hour, SUM(total) as revenue "
                  "FROM orders "
                  "WHERE DATE(created_at) = DATE('now') AND status = 'Paid' "
                  "GROUP BY hour "
                  "ORDER BY hour ASC");

    if (query.exec()) {
        while (query.next()) {
            int row = m_hourlyRevenueTable->rowCount();
            m_hourlyRevenueTable->insertRow(row);
            
            QString hour = query.value("hour").toString() + ":00";
            m_hourlyRevenueTable->setItem(row, 0, new QTableWidgetItem(hour));
            
            double revenue = query.value("revenue").toInt() / 100.0;
            m_hourlyRevenueTable->setItem(row, 1, new QTableWidgetItem(QString("$%1").arg(revenue, 0, 'f', 2)));
        }
    }
}