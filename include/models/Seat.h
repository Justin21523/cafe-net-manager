#pragma once

#include <QString>
#include <QDateTime>
#include <vector>

// Enum class provides strong typing. 
// You cannot accidentally compare SeatStatus with an integer.
enum class SeatStatus {
    Available,
    Occupied,
    Reserved,
    Cleaning,
    Disabled
};

// Helper namespace to convert between Enum and QString
namespace SeatStatusHelper {
    QString toString(SeatStatus status);
    SeatStatus fromString(const QString &str);
}

// Value Object: Represents a seat in the cafe.
// It does NOT inherit from QObject because it doesn't need signals/slots or dynamic properties.
// It's just a plain data structure.
struct Seat {
    int id = -1;
    QString code;
    QString name;
    QString area;
    QString type;
    
    // Position and size for the visual map (Phase 4)
    int x = 0;
    int y = 0;
    int width = 100;
    int height = 100;
    
    SeatStatus status = SeatStatus::Available;
    int capacity = 2;
    bool hasPowerOutlet = false;
    bool isQuietZone = false;
    QString note;
    
    QDateTime createdAt;
    QDateTime updatedAt;
};