#pragma once
#include <QObject>
#include <QSqlDatabase>
#include <QString>

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool initialize(const QString &dbPath);
    QSqlDatabase database() const;
    bool isOpen() const;
    void closeDatabase();

private:
    bool initializeSchema(); // Now handles migrations
    bool seedDemoData();
    bool migrateToV2();      // New migration step
    
    QSqlDatabase m_db;
    QString m_dbPath;
};