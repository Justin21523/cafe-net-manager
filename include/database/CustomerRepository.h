#pragma once

#include "models/Customer.h"
#include <vector>
#include <QString>

class DatabaseManager;

class CustomerRepository {
public:
    explicit CustomerRepository(DatabaseManager *dbManager);

    int createCustomer(const Customer &customer);
    bool updateCustomer(const Customer &customer);
    Customer getCustomerById(int id);
    std::vector<Customer> searchCustomers(const QString &query);
    std::vector<Customer> getAllCustomers();

private:
    DatabaseManager *m_dbManager;
};