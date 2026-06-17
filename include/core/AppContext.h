#pragma once

#include <QObject>

class DatabaseManager;
class SeatRepository;
class SeatService;

class AppContext : public QObject {
    Q_OBJECT

public:
    explicit AppContext(QObject *parent = nullptr);
    ~AppContext();

    DatabaseManager* databaseManager() const;
    SeatService* seatService() const;

private:
    DatabaseManager *m_databaseManager;
    SeatRepository *m_seatRepository;
    SeatService *m_seatService;
};