#pragma once

#include <QWidget>

class QLineEdit;
class QListWidget;
class CustomerRepository;

class CustomerSearchWidget : public QWidget {
    Q_OBJECT

public:
    explicit CustomerSearchWidget(CustomerRepository *repo, QWidget *parent = nullptr);

signals:
    void customerSelected(int customerId);

private slots:
    void onSearchTextChanged(const QString &text);

private:
    void setupUI();

    CustomerRepository *m_repo;
    QLineEdit *m_searchInput;
    QListWidget *m_resultsList;
};