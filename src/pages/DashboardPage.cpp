#include "pages/DashboardPage.h"
#include "widgets/DashboardWidget.h"
#include <QVBoxLayout>

DashboardPage::DashboardPage(DatabaseManager* dbManager, QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    m_dashboardWidget = new DashboardWidget(dbManager, this);
    layout->addWidget(m_dashboardWidget);
}

void DashboardPage::refreshData() {
    if (m_dashboardWidget) m_dashboardWidget->refreshData();
}