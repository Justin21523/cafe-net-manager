#include "widgets/admin/CustomerAdminWidget.h"
#include "database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QSqlTableModel>
#include <QLabel>

CustomerAdminWidget::CustomerAdminWidget(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager) {
    setupUI();
    refreshTable();
}

void CustomerAdminWidget::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("👤 Customer Database (Read-Only View for now)", this));
    
    m_tableView = new QTableView(this);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->verticalHeader()->setVisible(false);
    layout->addWidget(m_tableView);
}

void CustomerAdminWidget::refreshTable() {
    if (m_model) delete m_model;
    m_model = new QSqlTableModel(this, m_dbManager->database());
    m_model->setTable("customers");
    m_model->select();
    m_tableView->setModel(m_model);
    m_tableView->resizeColumnsToContents();
}