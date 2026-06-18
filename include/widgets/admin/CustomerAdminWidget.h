#pragma once
#include <QWidget>
class QTableView;
class QSqlTableModel;
class DatabaseManager;

class CustomerAdminWidget : public QWidget {
    Q_OBJECT
public:
    explicit CustomerAdminWidget(DatabaseManager *dbManager, QWidget *parent = nullptr);
private:
    void setupUI();
    void refreshTable();
    DatabaseManager *m_dbManager;
    QTableView *m_tableView;
    QSqlTableModel *m_model;
};