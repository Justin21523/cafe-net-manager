#pragma once
#include <QWidget>

class QTableView;
class QSqlTableModel;
class DatabaseManager;

class SeatAdminWidget : public QWidget {
    Q_OBJECT
public:
    explicit SeatAdminWidget(DatabaseManager *dbManager, QWidget *parent = nullptr);

signals:
    void seatsUpdated();

private slots:
    void addSeat();
    void editSeat();
    void deleteSeat();
    void refreshTable();

private:
    void setupUI();
    void showEditDialog(int row);

    DatabaseManager *m_dbManager;
    QTableView *m_tableView;
    QSqlTableModel *m_model = nullptr;
};
