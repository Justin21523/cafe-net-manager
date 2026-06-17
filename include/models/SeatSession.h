#pragma once

#include <QString>
#include <QDateTime>

enum class SessionMode {
    Cafe,
    InternetCafe,
    Study
};

enum class SessionStatus {
    Active,
    Completed
};

struct SeatSession {
    int id = -1;
    int seatId = -1;
    QString customerName;
    int guestCount = 1;
    QDateTime startTime;
    QDateTime endTime;
    SessionMode mode = SessionMode::Cafe;
    SessionStatus status = SessionStatus::Active;
    QString note;
};