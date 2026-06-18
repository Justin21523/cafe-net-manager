#include "services/SeatSessionService.h"
#include "database/SeatRepository.h"
#include "database/SeatSessionRepository.h"
#include "database/DatabaseManager.h" 
#include "models/SeatSession.h"
#include "utils/Logger.h"

#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>


SeatSessionService::SeatSessionService(SeatRepository *seatRepo, SeatSessionRepository *sessionRepo)
    : m_seatRepo(seatRepo), m_sessionRepo(sessionRepo) {}

bool SeatSessionService::startSession(int seatId, int prepaidMinutes, int hourlyRate) {
    // Check if there's already an active session
    if (m_sessionRepo->getActiveSessionBySeatId(seatId).has_value()) {
        Logger::warning("Seat already has an active session.");
        return false;
    }

    SeatSession newSession;
    newSession.seatId = seatId;
    newSession.customerName = "Guest";
    newSession.guestCount = 1;
    newSession.startTime = QDateTime::currentDateTime();
    newSession.prepaidMinutes = prepaidMinutes;
    newSession.hourlyRate = hourlyRate;

    // Calculate expected end time for Internet Cafe mode
    if (prepaidMinutes > 0) {
        newSession.expectedEndTime = newSession.startTime.addSecs(prepaidMinutes * 60);
        newSession.mode = SessionMode::InternetCafe;
    } else {
        newSession.mode = SessionMode::Cafe;
    }

    int sessionId = m_sessionRepo->createSession(newSession);
    if (sessionId == -1) return false;

    // Update seat status to Occupied
    if (!m_seatRepo->updateStatus(seatId, SeatStatus::Occupied)) {
        return false;
    }

    Logger::info("Session started for seat " + QString::number(seatId));
    return true;
}

bool SeatSessionService::extendSession(int sessionId, int additionalMinutes) {
    // Get current session (simplified - in real app, fetch from DB)
    // For now, just update the expected end time
    
    QSqlQuery query(m_sessionRepo->databaseManager()->database());
    query.prepare("UPDATE seat_sessions SET expected_end_time = datetime(expected_end_time, '+' || :mins || ' minutes'), "
                  "updated_at = CURRENT_TIMESTAMP WHERE id = :id");
    query.bindValue(":mins", additionalMinutes);
    query.bindValue(":id", sessionId);

    if (!query.exec()) {
        Logger::error("Failed to extend session: " + query.lastError().text());
        return false;
    }

    Logger::info("Session " + QString::number(sessionId) + " extended by " + 
                 QString::number(additionalMinutes) + " minutes");
    return true;
}

bool SeatSessionService::updateSessionTime(int sessionId) {
    // This would be called periodically to update elapsed time
    // For now, just a placeholder
    return true;
}

std::optional<SeatSession> SeatSessionService::getActiveSession(int seatId) const {
    return m_sessionRepo->getActiveSessionBySeatId(seatId);
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
