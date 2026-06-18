#pragma once
#include <QWidget>
#include <QModelIndex>

class QTableView;
class QListView;
class QSqlTableModel;
class QSqlRelationalTableModel;
class DatabaseManager;

class MenuAdminWidget : public QWidget {
    Q_OBJECT
public:
    explicit MenuAdminWidget(DatabaseManager *dbManager, QWidget *parent = nullptr);

signals:
    void menuUpdated();

private slots:
    void onCategorySelected(const QModelIndex &index);
    void addCategory();
    void deleteCategory();
    void addItem();
    void deleteItem();

private:
    void setupUI();
    void refreshCategories();
    void refreshItems(int categoryId);

    DatabaseManager *m_dbManager;
    
    QTableView *m_catView;
    QSqlTableModel *m_catModel = nullptr;
    
    QTableView *m_itemView;
    QSqlTableModel *m_itemModel = nullptr;
    
    int m_currentCatId = -1;
};
