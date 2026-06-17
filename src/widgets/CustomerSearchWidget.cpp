#include "widgets/CustomerSearchWidget.h"
#include "database/CustomerRepository.h"
#include "models/Customer.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>

CustomerSearchWidget::CustomerSearchWidget(CustomerRepository *repo, QWidget *parent)
    : QWidget(parent), m_repo(repo) {
    setupUI();
}

void CustomerSearchWidget::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *label = new QLabel("Search Customer:", this);
    layout->addWidget(label);

    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Type name or phone...");
    layout->addWidget(m_searchInput);

    m_resultsList = new QListWidget(this);
    layout->addWidget(m_resultsList);

    connect(m_searchInput, &QLineEdit::textChanged, this, &CustomerSearchWidget::onSearchTextChanged);
    connect(m_resultsList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        int customerId = item->data(Qt::UserRole).toInt();
        emit customerSelected(customerId);
    });
}

void CustomerSearchWidget::onSearchTextChanged(const QString &text) {
    m_resultsList->clear();
    
    if (text.length() < 2) return;

    std::vector<Customer> customers = m_repo->searchCustomers(text);
    
    for (const auto &customer : customers) {
        QString displayText = QString("%1 (%2) - %3 visits")
            .arg(customer.name)
            .arg(customer.phone)
            .arg(customer.totalVisits);
        
        QListWidgetItem *item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, customer.id);
        m_resultsList->addItem(item);
    }
}