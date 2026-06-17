#pragma once

#include "models/SeatSession.h"
#include <optional>

class DatabaseManager;

class SeatSessionRepository {
public:
    explicit SeatSessionRepository(DatabaseManager *dbManager);

    int createSession(const SeatSession &session);
    bool endSession(int sessionId, const QDateTime &endTime);
    std::optional<SeatSession> getActiveSessionBySeatId(int seatId);
    DatabaseManager* databaseManager() const { return m_dbManager; }
    
private:
    DatabaseManager *m_dbManager;
};