#include "widgets/AdminPage.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>

AdminPage::AdminPage(QWidget *parent)
    : QWidget(parent) {
    setupUI();
}

void AdminPage::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Admin Panel", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    m_tabWidget = new QTabWidget(this);
    layout->addWidget(m_tabWidget);
}