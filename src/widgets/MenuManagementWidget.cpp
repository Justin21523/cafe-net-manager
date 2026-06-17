#include "widgets/MenuManagementWidget.h"
#include "database/DatabaseManager.h"

#include <QVBoxLayout>
#include <QTableView>
#include <QPushButton>
#include <QSqlTableModel>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QLineEdit>

MenuManagementWidget::MenuManagementWidget(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager) {
    setupUI();
    refreshTable();
}

void MenuManagementWidget::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_addBtn = new QPushButton("Add Menu Item", this);
    m_refreshBtn = new QPushButton("Refresh", this);

    buttonLayout->addWidget(m_addBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_refreshBtn);
    layout->addLayout(buttonLayout);

    m_tableView = new QTableView(this);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(m_tableView);

    connect(m_addBtn, &QPushButton::clicked, this, &MenuManagementWidget::addItem);
    connect(m_refreshBtn, &QPushButton::clicked, this, &MenuManagementWidget::refreshTable);
}

void MenuManagementWidget::refreshTable() {
    QSqlTableModel *model = new QSqlTableModel(this, m_dbManager->database());
    model->setTable("menu_items");
    model->select();

    // Hide internal columns
    model->removeColumn(model->fieldIndex("cost"));
    model->removeColumn(model->fieldIndex("created_at"));
    model->removeColumn(model->fieldIndex("updated_at"));

    m_tableView->setModel(model);
    m_tableView->resizeColumnsToContents();
}

void MenuManagementWidget::addItem() {
    bool ok;
    QString name = QInputDialog::getText(this, "Add Menu Item", "Item Name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    double price = QInputDialog::getDouble(this, "Price", "Price ($):", 0.0, 0.0, 1000.0, 2, &ok);
    if (!ok) return;

    int categoryId = QInputDialog::getInt(this, "Category", "Category ID (1-5):", 1, 1, 5, 1, &ok);
    if (!ok) return;

    QSqlQuery query(m_dbManager->database());
    query.prepare("INSERT INTO menu_items (category_id, name, price, is_active, is_available) VALUES (:cat_id, :name, :price, 1, 1)");
    query.bindValue(":cat_id", categoryId);
    query.bindValue(":name", name);
    query.bindValue(":price", static_cast<int>(price * 100)); // Convert to cents

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Menu item added!");
        refreshTable();
    } else {
        QMessageBox::critical(this, "Error", "Failed: " + query.lastError().text());
    }
}