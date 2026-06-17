#pragma once

#include <QWidget>

class QTabWidget;
class QPushButton;

class AdminPage : public QWidget {
    Q_OBJECT

public:
    explicit AdminPage(QWidget *parent = nullptr);

private:
    void setupUI();

    QTabWidget *m_tabWidget;
    QPushButton *m_seatManagementBtn;
    QPushButton *m_menuManagementBtn;
};