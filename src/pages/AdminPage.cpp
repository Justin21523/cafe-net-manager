#include "pages/AdminPage.h"
#include "widgets/admin/SeatAdminWidget.h"
#include "widgets/admin/MenuAdminWidget.h"
#include "widgets/admin/CustomerAdminWidget.h"
#include "database/DatabaseManager.h"

#include <QVBoxLayout>
#include <QTabWidget>
#include <QLabel>

// 修改建構子以接收 DatabaseManager
AdminPage::AdminPage(DatabaseManager *dbManager, QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *title = new QLabel("⚙️ Admin Center", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; padding: 15px; background-color: #2b2b2b; color: white;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    m_tabWidget = new QTabWidget(this);
    
    // 1. Seat Admin
    SeatAdminWidget *seatAdmin = new SeatAdminWidget(dbManager, this);
    connect(seatAdmin, &SeatAdminWidget::seatsUpdated, this, [this]() { emit dataUpdated("seats"); });
    m_tabWidget->addTab(seatAdmin, "🪑 Seats");

    // 2. Menu Admin
    MenuAdminWidget *menuAdmin = new MenuAdminWidget(dbManager, this);
    connect(menuAdmin, &MenuAdminWidget::menuUpdated, this, [this]() { emit dataUpdated("menu"); });
    m_tabWidget->addTab(menuAdmin, "🍔 Menu & Categories");

    // 3. Customer Admin
    CustomerAdminWidget *custAdmin = new CustomerAdminWidget(dbManager, this);
    m_tabWidget->addTab(custAdmin, "👤 Customers");

    layout->addWidget(m_tabWidget);
}