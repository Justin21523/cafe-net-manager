#pragma once

#include <QWidget>

class QTableView;
class QPushButton;
class DatabaseManager;

class MenuManagementWidget : public QWidget {
    Q_OBJECT

public:
    explicit MenuManagementWidget(DatabaseManager *dbManager, QWidget *parent = nullptr);

private slots:
    void addItem();
    void refreshTable();

private:
    void setupUI();

    DatabaseManager *m_dbManager;
    QTableView *m_tableView;
    QPushButton *m_addBtn;
    QPushButton *m_refreshBtn;
};