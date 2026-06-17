#include "services/SeatService.h"
#include "database/SeatRepository.h"
#include "utils/Logger.h"

SeatService::SeatService(SeatRepository *repository)
    : m_repository(repository) {
}

std::vector<Seat> SeatService::loadAllSeats() {
    Logger::info("SeatService: Loading all seats...");
    return m_repository->getAllSeats();
}