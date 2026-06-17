#include "services/SeatSessionService.h"
#include "database/SeatRepository.h"
#include "database/SeatSessionRepository.h"
#include "models/SeatSession.h"
#include "utils/Logger.h"

#include <QDateTime>

SeatSessionService::SeatSessionService(SeatRepository *seatRepo, SeatSessionRepository *sessionRepo)
    : m_seatRepo(seatRepo), m_sessionRepo(sessionRepo) {}

bool SeatSessionService::startSession(int seatId) {
    // Check if there's already an active session
    if (m_sessionRepo->getActiveSessionBySeatId(seatId).has_value()) {
        Logger::warning("Seat already has an active session.");
        return false;
    }

    SeatSession newSession;
    newSession.seatId = seatId;
    newSession.customerName = "Guest"; // Default for now
    newSession.guestCount = 1;
    newSession.startTime = QDateTime::currentDateTime();

    int sessionId = m_sessionRepo->createSession(newSession);
    if (sessionId == -1) return false;

    // Update seat status to Occupied
    if (!m_seatRepo->updateStatus(seatId, SeatStatus::Occupied)) {
        return false;
    }

    Logger::info("Session started for seat " + QString::number(seatId));
    return true;
}

bool SeatSessionService::endSession(int seatId) {
    auto sessionOpt = m_sessionRepo->getActiveSessionBySeatId(seatId);
    if (!sessionOpt.has_value()) {
        Logger::warning("No active session found for seat.");
        return false;
    }

    if (!m_sessionRepo->endSession(sessionOpt->id, QDateTime::currentDateTime())) {
        return false;
    }

    // Update seat status back to Available
    if (!m_seatRepo->updateStatus(seatId, SeatStatus::Available)) {
        return false;
    }

    Logger::info("Session ended for seat " + QString::number(seatId));
    return true;
}