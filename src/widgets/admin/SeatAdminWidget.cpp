#include "widgets/admin/SeatAdminWidget.h"
#include "database/DatabaseManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>

SeatAdminWidget::SeatAdminWidget(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager) {
    setupUI();
    refreshTable();
}

void SeatAdminWidget::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *addBtn = new QPushButton("➕ Add Seat", this);
    QPushButton *editBtn = new QPushButton("✏️ Edit", this);
    QPushButton *delBtn = new QPushButton("🗑️ Delete", this);
    QPushButton *refreshBtn = new QPushButton("🔄 Refresh", this);

    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(editBtn);
    btnLayout->addWidget(delBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(refreshBtn);
    layout->addLayout(btnLayout);

    m_tableView = new QTableView(this);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->verticalHeader()->setVisible(false);
    layout->addWidget(m_tableView);

    connect(addBtn, &QPushButton::clicked, this, &SeatAdminWidget::addSeat);
    connect(editBtn, &QPushButton::clicked, this, &SeatAdminWidget::editSeat);
    connect(delBtn, &QPushButton::clicked, this, &SeatAdminWidget::deleteSeat);
    connect(refreshBtn, &QPushButton::clicked, this, &SeatAdminWidget::refreshTable);
    connect(m_tableView, &QTableView::doubleClicked, this, [this](const QModelIndex &) { editSeat(); });
}

void SeatAdminWidget::refreshTable() {
    if (m_model) {
        m_model->revertAll();
        delete m_model;
    }

    m_model = new QSqlTableModel(this, m_dbManager->database());
    m_model->setTable("seats");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();

    // Hide graphical/internal columns
    m_model->removeColumn(m_model->fieldIndex("x"));
    m_model->removeColumn(m_model->fieldIndex("y"));
    m_model->removeColumn(m_model->fieldIndex("width"));
    m_model->removeColumn(m_model->fieldIndex("height"));
    m_model->removeColumn(m_model->fieldIndex("created_at"));
    m_model->removeColumn(m_model->fieldIndex("updated_at"));

    m_tableView->setModel(m_model);
    m_tableView->resizeColumnsToContents();
    
    // Set friendly headers
    m_model->setHeaderData(m_model->fieldIndex("code"), Qt::Horizontal, "Code");
    m_model->setHeaderData(m_model->fieldIndex("name"), Qt::Horizontal, "Display Name");
    m_model->setHeaderData(m_model->fieldIndex("area"), Qt::Horizontal, "Area");
    m_model->setHeaderData(m_model->fieldIndex("capacity"), Qt::Horizontal, "Capacity");
    m_model->setHeaderData(m_model->fieldIndex("status"), Qt::Horizontal, "Status");
}

void SeatAdminWidget::addSeat() {
    // Insert a new row with default values
    int row = m_model->rowCount();
    m_model->insertRow(row);
    m_model->setData(m_model->index(row, m_model->fieldIndex("code")), "NEW");
    m_model->setData(m_model->index(row, m_model->fieldIndex("name")), "New Seat");
    m_model->setData(m_model->index(row, m_model->fieldIndex("area")), "Main");
    m_model->setData(m_model->index(row, m_model->fieldIndex("capacity")), 2);
    m_model->setData(m_model->index(row, m_model->fieldIndex("status")), "Available");
    
    showEditDialog(row);
}

void SeatAdminWidget::editSeat() {
    QModelIndex index = m_tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select a seat to edit.");
        return;
    }
    showEditDialog(index.row());
}

void SeatAdminWidget::showEditDialog(int row) {
    QDialog dialog(this);
    dialog.setWindowTitle("Edit Seat");
    dialog.setMinimumWidth(350);
    QFormLayout form(&dialog);

    auto getVal = [&](const QString &col) { return m_model->data(m_model->index(row, m_model->fieldIndex(col))).toString(); };
    auto setVal = [&](const QString &col, const QVariant &val) { m_model->setData(m_model->index(row, m_model->fieldIndex(col)), val); };

    QLineEdit *codeEdit = new QLineEdit(getVal("code"), &dialog);
    QLineEdit *nameEdit = new QLineEdit(getVal("name"), &dialog);
    QLineEdit *areaEdit = new QLineEdit(getVal("area"), &dialog);
    QSpinBox *capSpin = new QSpinBox(&dialog); capSpin->setRange(1, 20); capSpin->setValue(getVal("capacity").toInt());
    
    QComboBox *statusCombo = new QComboBox(&dialog);
    statusCombo->addItems({"Available", "Occupied", "Reserved", "Cleaning", "Disabled"});
    statusCombo->setCurrentText(getVal("status"));

    form.addRow("Code:", codeEdit);
    form.addRow("Name:", nameEdit);
    form.addRow("Area:", areaEdit);
    form.addRow("Capacity:", capSpin);
    form.addRow("Status:", statusCombo);

    QDialogButtonBox *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(btnBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    form.addRow(btnBox);

    if (dialog.exec() == QDialog::Accepted) {
        setVal("code", codeEdit->text());
        setVal("name", nameEdit->text());
        setVal("area", areaEdit->text());
        setVal("capacity", capSpin->value());
        setVal("status", statusCombo->currentText());
        
        if (m_model->submitAll()) {
            emit seatsUpdated();
        } else {
            QMessageBox::critical(this, "Error", "Failed to save: " + m_model->lastError().text());
            m_model->revertAll();
        }
    } else {
        m_model->revertRow(row);
    }
}

void SeatAdminWidget::deleteSeat() {
    QModelIndex index = m_tableView->currentIndex();
    if (!index.isValid()) return;

    if (QMessageBox::question(this, "Confirm", "Delete this seat?") == QMessageBox::Yes) {
        m_model->removeRow(index.row());
        if (m_model->submitAll()) {
            emit seatsUpdated();
        } else {
            QMessageBox::critical(this, "Error", m_model->lastError().text());
            m_model->revertAll();
        }
    }
}
