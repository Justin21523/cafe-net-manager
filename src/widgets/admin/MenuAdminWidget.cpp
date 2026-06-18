#include "widgets/admin/MenuAdminWidget.h"
#include "database/DatabaseManager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSplitter>
#include <QTableView>
#include <QHeaderView>
#include <QSqlTableModel>
#include <QSqlError>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>

MenuAdminWidget::MenuAdminWidget(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager) {
    setupUI();
    refreshCategories();
}

void MenuAdminWidget::setupUI() {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    // --- Left: Categories ---
    QWidget *catWidget = new QWidget(this);
    QVBoxLayout *catLayout = new QVBoxLayout(catWidget);
    catLayout->setContentsMargins(0,0,0,0);
    
    m_catView = new QTableView(this);
    m_catView->setMaximumWidth(250);
    m_catView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_catView->verticalHeader()->setVisible(false);
    catLayout->addWidget(m_catView);

    QHBoxLayout *catBtnLayout = new QHBoxLayout();
    QPushButton *addCatBtn = new QPushButton("➕ Add", this);
    QPushButton *delCatBtn = new QPushButton("🗑️ Del", this);
    catBtnLayout->addWidget(addCatBtn);
    catBtnLayout->addWidget(delCatBtn);
    catLayout->addLayout(catBtnLayout);
    splitter->addWidget(catWidget);

    // --- Right: Items ---
    QWidget *itemWidget = new QWidget(this);
    QVBoxLayout *itemLayout = new QVBoxLayout(itemWidget);
    itemLayout->setContentsMargins(0,0,0,0);

    m_itemView = new QTableView(this);
    m_itemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_itemView->verticalHeader()->setVisible(false);
    itemLayout->addWidget(m_itemView);

    QHBoxLayout *itemBtnLayout = new QHBoxLayout();
    QPushButton *addItemBtn = new QPushButton("➕ Add Item", this);
    QPushButton *delItemBtn = new QPushButton("🗑️ Del Item", this);
    itemBtnLayout->addWidget(addItemBtn);
    itemBtnLayout->addWidget(delItemBtn);
    itemLayout->addLayout(itemBtnLayout);
    splitter->addWidget(itemWidget);

    mainLayout->addWidget(splitter);

    // Connections
    connect(addCatBtn, &QPushButton::clicked, this, &MenuAdminWidget::addCategory);
    connect(delCatBtn, &QPushButton::clicked, this, &MenuAdminWidget::deleteCategory);
    connect(addItemBtn, &QPushButton::clicked, this, &MenuAdminWidget::addItem);
    connect(delItemBtn, &QPushButton::clicked, this, &MenuAdminWidget::deleteItem);
}

void MenuAdminWidget::refreshCategories() {
    if (m_catModel) delete m_catModel;
    m_catModel = new QSqlTableModel(this, m_dbManager->database());
    m_catModel->setTable("menu_categories");
    m_catModel->select();
    m_catModel->setHeaderData(m_catModel->fieldIndex("name"), Qt::Horizontal, "Category Name");
    
    m_catView->setModel(m_catModel);
    m_catView->hideColumn(m_catModel->fieldIndex("id"));
    m_catView->hideColumn(m_catModel->fieldIndex("sort_order"));
    m_catView->hideColumn(m_catModel->fieldIndex("created_at"));
    m_catView->hideColumn(m_catModel->fieldIndex("updated_at"));
    m_catView->resizeColumnsToContents();

    if (m_catView->selectionModel()) {
        connect(m_catView->selectionModel(), &QItemSelectionModel::currentChanged,
                this, &MenuAdminWidget::onCategorySelected, Qt::UniqueConnection);
    }
}

void MenuAdminWidget::onCategorySelected(const QModelIndex &index) {
    if (!index.isValid()) return;
    int row = index.row();
    m_currentCatId = m_catModel->data(m_catModel->index(row, m_catModel->fieldIndex("id"))).toInt();
    refreshItems(m_currentCatId);
}

void MenuAdminWidget::refreshItems(int categoryId) {
    if (m_itemModel) delete m_itemModel;
    m_itemModel = new QSqlTableModel(this, m_dbManager->database());
    m_itemModel->setTable("menu_items");
    m_itemModel->setFilter(QString("category_id = %1").arg(categoryId));
    m_itemModel->select();

    m_itemModel->setHeaderData(m_itemModel->fieldIndex("name"), Qt::Horizontal, "Item Name");
    m_itemModel->setHeaderData(m_itemModel->fieldIndex("price"), Qt::Horizontal, "Price (cents)");
    m_itemModel->setHeaderData(m_itemModel->fieldIndex("is_available"), Qt::Horizontal, "Available");

    m_itemView->setModel(m_itemModel);
    m_itemView->hideColumn(m_itemModel->fieldIndex("id"));
    m_itemView->hideColumn(m_itemModel->fieldIndex("category_id"));
    m_itemView->hideColumn(m_itemModel->fieldIndex("description"));
    m_itemView->hideColumn(m_itemModel->fieldIndex("cost"));
    m_itemView->hideColumn(m_itemModel->fieldIndex("is_active"));
    m_itemView->hideColumn(m_itemModel->fieldIndex("sort_order"));
    m_itemView->hideColumn(m_itemModel->fieldIndex("created_at"));
    m_itemView->hideColumn(m_itemModel->fieldIndex("updated_at"));
    m_itemView->resizeColumnsToContents();
}

void MenuAdminWidget::addCategory() {
    bool ok;
    QString name = QInputDialog::getText(this, "Add Category", "Name:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        int row = m_catModel->rowCount();
        m_catModel->insertRow(row);
        m_catModel->setData(m_catModel->index(row, m_catModel->fieldIndex("name")), name);
        if (m_catModel->submitAll()) {
            refreshCategories();
            emit menuUpdated();
        }
    }
}

void MenuAdminWidget::deleteCategory() {
    QModelIndex index = m_catView->currentIndex();
    if (!index.isValid()) return;
    if (QMessageBox::question(this, "Confirm", "Delete category?") == QMessageBox::Yes) {
        m_catModel->removeRow(index.row());
        if (m_catModel->submitAll()) {
            refreshCategories();
            emit menuUpdated();
        }
    }
}

void MenuAdminWidget::addItem() {
    if (m_currentCatId == -1) {
        QMessageBox::warning(this, "Warning", "Select a category first.");
        return;
    }
    bool ok;
    QString name = QInputDialog::getText(this, "Add Item", "Item Name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;
    
    int price = QInputDialog::getInt(this, "Price", "Price (in dollars, will convert to cents):", 0, 0, 10000, 1, &ok);
    if (!ok) return;

    int row = m_itemModel->rowCount();
    m_itemModel->insertRow(row);
    m_itemModel->setData(m_itemModel->index(row, m_itemModel->fieldIndex("category_id")), m_currentCatId);
    m_itemModel->setData(m_itemModel->index(row, m_itemModel->fieldIndex("name")), name);
    m_itemModel->setData(m_itemModel->index(row, m_itemModel->fieldIndex("price")), price * 100);
    m_itemModel->setData(m_itemModel->index(row, m_itemModel->fieldIndex("is_available")), 1);
    m_itemModel->setData(m_itemModel->index(row, m_itemModel->fieldIndex("is_active")), 1);

    if (m_itemModel->submitAll()) {
        emit menuUpdated();
    } else {
        QMessageBox::critical(this, "Error", m_itemModel->lastError().text());
        m_itemModel->revertAll();
    }
}

void MenuAdminWidget::deleteItem() {
    QModelIndex index = m_itemView->currentIndex();
    if (!index.isValid()) return;
    if (QMessageBox::question(this, "Confirm", "Delete item?") == QMessageBox::Yes) {
        m_itemModel->removeRow(index.row());
        if (m_itemModel->submitAll()) {
            emit menuUpdated();
        }
    }
}
