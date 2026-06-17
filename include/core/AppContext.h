#pragma once

#include <QObject>

class DatabaseManager;
class SeatRepository;
class SeatService;
class SeatSessionRepository;
class SeatSessionService;

class AppContext : public QObject {
    Q_OBJECT

public:
    explicit AppContext(QObject *parent = nullptr);
    ~AppContext();

    DatabaseManager* databaseManager() const;
    SeatService* seatService() const;
    SeatSessionService* seatSessionService() const;

private:
    DatabaseManager *m_databaseManager;
    SeatRepository *m_seatRepository;
    SeatService *m_seatService;
    SeatSessionRepository *m_sessionRepository;
    SeatSessionService *m_sessionService;
};