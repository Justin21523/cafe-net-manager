#pragma once
#include <QWidget>

class QTabWidget;
class DatabaseManager;

class AdminPage : public QWidget {
    Q_OBJECT
public:
    explicit AdminPage(DatabaseManager *dbManager, QWidget *parent = nullptr);

signals:
    void dataUpdated(const QString &module); // "seats", "menu", "customers"

private:
    QTabWidget *m_tabWidget;
};
