#pragma once

#include "models/Seat.h"
#include <vector>

class SeatRepository;

class SeatService {
public:
    explicit SeatService(SeatRepository *repository);

    std::vector<Seat> loadAllSeats();

private:
    SeatRepository *m_repository;
};