#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    // Opens the database connection and initializes schema/data
    bool initialize(const QString &dbPath);
    
    // Returns the active database connection
    QSqlDatabase database() const;
    
    bool isOpen() const;
    void closeDatabase();

private:
    // Internal methods for setup
    bool initializeSchema();
    bool seedDemoData();

    QSqlDatabase m_db;
    QString m_dbPath;
};