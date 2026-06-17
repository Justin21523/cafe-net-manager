#include "widgets/SeatManagementWidget.h"
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

SeatManagementWidget::SeatManagementWidget(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager) {
    setupUI();
    refreshTable();
}

void SeatManagementWidget::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_addBtn = new QPushButton("Add Seat", this);
    m_editBtn = new QPushButton("Edit Seat", this);
    m_deleteBtn = new QPushButton("Delete Seat", this);
    m_refreshBtn = new QPushButton("Refresh", this);

    buttonLayout->addWidget(m_addBtn);
    buttonLayout->addWidget(m_editBtn);
    buttonLayout->addWidget(m_deleteBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_refreshBtn);
    layout->addLayout(buttonLayout);

    // Table
    m_tableView = new QTableView(this);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(m_tableView);

    // Connections
    connect(m_addBtn, &QPushButton::clicked, this, &SeatManagementWidget::addSeat);
    connect(m_editBtn, &QPushButton::clicked, this, &SeatManagementWidget::editSeat);
    connect(m_deleteBtn, &QPushButton::clicked, this, &SeatManagementWidget::deleteSeat);
    connect(m_refreshBtn, &QPushButton::clicked, this, &SeatManagementWidget::refreshTable);
}

void SeatManagementWidget::refreshTable() {
    QSqlTableModel *model = new QSqlTableModel(this, m_dbManager->database());
    model->setTable("seats");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    // Hide some columns for better display
    model->removeColumn(model->fieldIndex("x"));
    model->removeColumn(model->fieldIndex("y"));
    model->removeColumn(model->fieldIndex("width"));
    model->removeColumn(model->fieldIndex("height"));
    model->removeColumn(model->fieldIndex("created_at"));
    model->removeColumn(model->fieldIndex("updated_at"));

    m_tableView->setModel(model);
    m_tableView->resizeColumnsToContents();
}

void SeatManagementWidget::addSeat() {
    bool ok;
    QString code = QInputDialog::getText(this, "Add Seat", "Seat Code:", QLineEdit::Normal, "", &ok);
    if (!ok || code.isEmpty()) return;

    QSqlQuery query(m_dbManager->database());
    query.prepare("INSERT INTO seats (code, name, area, capacity, status) VALUES (:code, :name, :area, :capacity, :status)");
    query.bindValue(":code", code);
    query.bindValue(":name", "Seat " + code);
    query.bindValue(":area", "Main");
    query.bindValue(":capacity", 2);
    query.bindValue(":status", "Available");

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Seat added successfully!");
        refreshTable();
    } else {
        QMessageBox::critical(this, "Error", "Failed to add seat: " + query.lastError().text());
    }
}

void SeatManagementWidget::editSeat() {
    int row = m_tableView->currentIndex().row();
    if (row < 0) {
        QMessageBox::warning(this, "Warning", "Please select a seat to edit.");
        return;
    }

    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(m_tableView->model());
    if (!model) return;

    QModelIndex idIndex = model->index(row, model->fieldIndex("id"));
    int seatId = model->data(idIndex).toInt();

    // Simple edit: just change capacity for demo
    bool ok;
    int newCapacity = QInputDialog::getInt(this, "Edit Seat", "New Capacity:", 2, 1, 20, 1, &ok);
    if (!ok) return;

    QSqlQuery query(m_dbManager->database());
    query.prepare("UPDATE seats SET capacity = :capacity WHERE id = :id");
    query.bindValue(":capacity", newCapacity);
    query.bindValue(":id", seatId);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Seat updated!");
        refreshTable();
    }
}

void SeatManagementWidget::deleteSeat() {
    int row = m_tableView->currentIndex().row();
    if (row < 0) {
        QMessageBox::warning(this, "Warning", "Please select a seat to delete.");
        return;
    }

    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(m_tableView->model());
    if (!model) return;

    QModelIndex idIndex = model->index(row, model->fieldIndex("id"));
    int seatId = model->data(idIndex).toInt();

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
        "Are you sure you want to delete this seat?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QSqlQuery query(m_dbManager->database());
        query.prepare("DELETE FROM seats WHERE id = :id");
        query.bindValue(":id", seatId);

        if (query.exec()) {
            QMessageBox::information(this, "Success", "Seat deleted!");
            refreshTable();
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete: " + query.lastError().text());
        }
    }
}