#pragma once

#include <QWidget>
#include <QPushButton>
#include <QList>

class SidebarWidget : public QWidget {
    Q_OBJECT

public:
    explicit SidebarWidget(QWidget *parent = nullptr);

signals:
    void navigateTo(int index);

private:
    void setupUI();
    QPushButton* createNavButton(const QString &text, int index);
    
    QList<QPushButton*> m_buttons;
};