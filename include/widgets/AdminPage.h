#pragma once

#include <QWidget>

class QTabWidget;
class QPushButton;
class DatabaseManager;

class AdminPage : public QWidget {
    Q_OBJECT

public:
    explicit AdminPage(DatabaseManager *dbManager, QWidget *parent = nullptr);

private:
    void setupUI();

    DatabaseManager *m_dbManager;
    QTabWidget *m_tabWidget;
    QPushButton *m_seatManagementBtn;
    QPushButton *m_menuManagementBtn;
};
