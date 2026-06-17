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

private:
    DatabaseManager *m_dbManager;
};