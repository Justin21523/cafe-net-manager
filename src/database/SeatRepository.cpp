#include "database/SeatRepository.h"
#include "database/DatabaseManager.h"
#include "utils/Logger.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

SeatRepository::SeatRepository(DatabaseManager *dbManager)
    : m_dbManager(dbManager) {
}

std::vector<Seat> SeatRepository::getAllSeats() {
    std::vector<Seat> seats;

    QSqlQuery query(m_dbManager->database());
    query.prepare("SELECT id, code, name, area, type, x, y, width, height, status, capacity, has_power_outlet, is_quiet_zone, note, created_at, updated_at FROM seats");

    if (!query.exec()) {
        Logger::error("Failed to fetch seats: " + query.lastError().text());
        return seats; // Return empty vector on error
    }

    while (query.next()) {
        Seat seat;
        seat.id = query.value("id").toInt();
        seat.code = query.value("code").toString();
        seat.name = query.value("name").toString();
        seat.area = query.value("area").toString();
        seat.type = query.value("type").toString();
        
        seat.x = query.value("x").toInt();
        seat.y = query.value("y").toInt();
        seat.width = query.value("width").toInt();
        seat.height = query.value("height").toInt();
        
        // Convert QString from DB to our enum class
        seat.status = SeatStatusHelper::fromString(query.value("status").toString());
        seat.capacity = query.value("capacity").toInt();
        
        // SQLite stores booleans as integers (0 or 1)
        seat.hasPowerOutlet = query.value("has_power_outlet").toBool();
        seat.isQuietZone = query.value("is_quiet_zone").toBool();
        
        seat.note = query.value("note").toString();
        
        seat.createdAt = query.value("created_at").toDateTime();
        seat.updatedAt = query.value("updated_at").toDateTime();

        seats.push_back(seat);
    }

    Logger::info("Loaded " + QString::number(seats.size()) + " seats from database.");
    return seats;
}

bool SeatRepository::updateStatus(int seatId, SeatStatus status) {
    QSqlQuery query(m_dbManager->database());
    query.prepare("UPDATE seats SET status = :status, updated_at = CURRENT_TIMESTAMP WHERE id = :id");
    query.bindValue(":status", SeatStatusHelper::toString(status));
    query.bindValue(":id", seatId);

    if (!query.exec()) {
        Logger::error("Failed to update seat status: " + query.lastError().text());
        return false;
    }
    return true;
}