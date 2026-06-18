#include "widgets/AdminPage.h"
#include "widgets/MenuManagementWidget.h"
#include "widgets/SeatManagementWidget.h"

#include <QFont>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>

AdminPage::AdminPage(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager) {
    setupUI();
}

void AdminPage::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Admin Panel", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(new SeatManagementWidget(m_dbManager, m_tabWidget), "Seats");
    m_tabWidget->addTab(new MenuManagementWidget(m_dbManager, m_tabWidget), "Menu");
    layout->addWidget(m_tabWidget);
}
