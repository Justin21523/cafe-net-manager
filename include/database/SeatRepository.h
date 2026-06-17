#pragma once

#include "models/Seat.h"
#include <vector>

class DatabaseManager;

class SeatRepository {
public:
    // We take a pointer to DatabaseManager. 
    // Repository doesn't own the DatabaseManager, it just uses it.
    explicit SeatRepository(DatabaseManager *dbManager);

    // Fetches all seats from the database
    std::vector<Seat> getAllSeats();

    bool updateStatus(int seatId, SeatStatus status);
    bool updateSeatPosition(int seatId, int x, int y);

private:
    DatabaseManager *m_dbManager;
};