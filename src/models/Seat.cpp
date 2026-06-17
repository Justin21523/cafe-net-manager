#include "models/Seat.h"

namespace SeatStatusHelper {

QString toString(SeatStatus status) {
    switch (status) {
        case SeatStatus::Available: return "Available";
        case SeatStatus::Occupied:  return "Occupied";
        case SeatStatus::Reserved:  return "Reserved";
        case SeatStatus::Cleaning:  return "Cleaning";
        case SeatStatus::Disabled:  return "Disabled";
        default: return "Unknown";
    }
}

SeatStatus fromString(const QString &str) {
    if (str == "Available") return SeatStatus::Available;
    if (str == "Occupied")  return SeatStatus::Occupied;
    if (str == "Reserved")  return SeatStatus::Reserved;
    if (str == "Cleaning")  return SeatStatus::Cleaning;
    if (str == "Disabled")  return SeatStatus::Disabled;
    return SeatStatus::Available; // Default fallback
}

} // namespace SeatStatusHelper