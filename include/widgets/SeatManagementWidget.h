#pragma once

#include <QWidget>

class QTableView;
class QPushButton;
class DatabaseManager;

class SeatManagementWidget : public QWidget {
    Q_OBJECT

public:
    explicit SeatManagementWidget(DatabaseManager *dbManager, QWidget *parent = nullptr);

private slots:
    void addSeat();
    void editSeat();
    void deleteSeat();
    void refreshTable();

private:
    void setupUI();

    DatabaseManager *m_dbManager;
    QTableView *m_tableView;
    QPushButton *m_addBtn;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
    QPushButton *m_refreshBtn;
};