#include "database/SeatSessionRepository.h"
#include "database/DatabaseManager.h"
#include "utils/Logger.h"

#include <QSqlQuery>
#include <QSqlError>

SeatSessionRepository::SeatSessionRepository(DatabaseManager *dbManager)
    : m_dbManager(dbManager) {}

int SeatSessionRepository::createSession(const SeatSession &session) {
    QSqlQuery query(m_dbManager->database());
    query.prepare("INSERT INTO seat_sessions (seat_id, customer_name, guest_count, start_time, mode, status) "
                  "VALUES (:seat_id, :customer_name, :guest_count, :start_time, :mode, :status)");
    query.bindValue(":seat_id", session.seatId);
    query.bindValue(":customer_name", session.customerName);
    query.bindValue(":guest_count", session.guestCount);
    query.bindValue(":start_time", session.startTime.toString(Qt::ISODate));
    query.bindValue(":mode", "Cafe"); // Simplified for Phase 5
    query.bindValue(":status", "Active");

    if (!query.exec()) {
        Logger::error("Failed to create session: " + query.lastError().text());
        return -1;
    }
    return query.lastInsertId().toInt();
}

bool SeatSessionRepository::endSession(int sessionId, const QDateTime &endTime) {
    QSqlQuery query(m_dbManager->database());
    query.prepare("UPDATE seat_sessions SET end_time = :end_time, status = 'Completed' WHERE id = :id");
    query.bindValue(":end_time", endTime.toString(Qt::ISODate));
    query.bindValue(":id", sessionId);

    if (!query.exec()) {
        Logger::error("Failed to end session: " + query.lastError().text());
        return false;
    }
    return true;
}

std::optional<SeatSession> SeatSessionRepository::getActiveSessionBySeatId(int seatId) {
    QSqlQuery query(m_dbManager->database());
    query.prepare("SELECT id, seat_id, customer_name, guest_count, start_time, mode, status FROM seat_sessions WHERE seat_id = :seat_id AND status = 'Active'");
    query.bindValue(":seat_id", seatId);

    if (!query.exec()) {
        Logger::error("Failed to get active session: " + query.lastError().text());
        return std::nullopt;
    }

    if (query.next()) {
        SeatSession session;
        session.id = query.value("id").toInt();
        session.seatId = query.value("seat_id").toInt();
        session.customerName = query.value("customer_name").toString();
        session.guestCount = query.value("guest_count").toInt();
        session.startTime = query.value("start_time").toDateTime();
        session.mode = SessionMode::Cafe; // Simplified
        session.status = SessionStatus::Active;
        return session;
    }
    return std::nullopt;
}