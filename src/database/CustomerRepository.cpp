#include "database/CustomerRepository.h"
#include "database/DatabaseManager.h"
#include "utils/Logger.h"

#include <QSqlQuery>
#include <QSqlError>

CustomerRepository::CustomerRepository(DatabaseManager *dbManager)
    : m_dbManager(dbManager) {}

int CustomerRepository::createCustomer(const Customer &customer) {
    QSqlQuery query(m_dbManager->database());
    query.prepare("INSERT INTO customers (name, phone, email, notes, total_visits, total_spent, created_at) "
                  "VALUES (:name, :phone, :email, :notes, :total_visits, :total_spent, :created_at)");
    query.bindValue(":name", customer.name);
    query.bindValue(":phone", customer.phone);
    query.bindValue(":email", customer.email);
    query.bindValue(":notes", customer.notes);
    query.bindValue(":total_visits", customer.totalVisits);
    query.bindValue(":total_spent", customer.totalSpent);
    query.bindValue(":created_at", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        Logger::error("Failed to create customer: " + query.lastError().text());
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool CustomerRepository::updateCustomer(const Customer &customer) {
    QSqlQuery query(m_dbManager->database());
    query.prepare("UPDATE customers SET name = :name, phone = :phone, email = :email, "
                  "notes = :notes, total_visits = :total_visits, total_spent = :total_spent, "
                  "last_visit = :last_visit WHERE id = :id");
    query.bindValue(":name", customer.name);
    query.bindValue(":phone", customer.phone);
    query.bindValue(":email", customer.email);
    query.bindValue(":notes", customer.notes);
    query.bindValue(":total_visits", customer.totalVisits);
    query.bindValue(":total_spent", customer.totalSpent);
    query.bindValue(":last_visit", QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":id", customer.id);

    if (!query.exec()) {
        Logger::error("Failed to update customer: " + query.lastError().text());
        return false;
    }
    return true;
}

Customer CustomerRepository::getCustomerById(int id) {
    Customer customer;
    QSqlQuery query(m_dbManager->database());
    query.prepare("SELECT * FROM customers WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        customer.id = query.value("id").toInt();
        customer.name = query.value("name").toString();
        customer.phone = query.value("phone").toString();
        customer.email = query.value("email").toString();
        customer.notes = query.value("notes").toString();
        customer.totalVisits = query.value("total_visits").toInt();
        customer.totalSpent = query.value("total_spent").toInt();
        customer.createdAt = query.value("created_at").toDateTime();
        customer.lastVisit = query.value("last_visit").toDateTime();
    }
    return customer;
}

std::vector<Customer> CustomerRepository::searchCustomers(const QString &queryStr) {
    std::vector<Customer> customers;
    QSqlQuery query(m_dbManager->database());
    query.prepare("SELECT * FROM customers WHERE name LIKE :query OR phone LIKE :query "
                  "ORDER BY last_visit DESC LIMIT 20");
    query.bindValue(":query", "%" + queryStr + "%");

    if (query.exec()) {
        while (query.next()) {
            Customer customer;
            customer.id = query.value("id").toInt();
            customer.name = query.value("name").toString();
            customer.phone = query.value("phone").toString();
            customer.totalVisits = query.value("total_visits").toInt();
            customer.totalSpent = query.value("total_spent").toInt();
            customers.push_back(customer);
        }
    }
    return customers;
}

std::vector<Customer> CustomerRepository::getAllCustomers() {
    std::vector<Customer> customers;
    QSqlQuery query(m_dbManager->database());
    query.prepare("SELECT * FROM customers ORDER BY name ASC");

    if (query.exec()) {
        while (query.next()) {
            Customer customer;
            customer.id = query.value("id").toInt();
            customer.name = query.value("name").toString();
            customer.phone = query.value("phone").toString();
            customer.totalVisits = query.value("total_visits").toInt();
            customer.totalSpent = query.value("total_spent").toInt();
            customers.push_back(customer);
        }
    }
    return customers;
}