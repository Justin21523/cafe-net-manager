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
    Completed,
    Expired
};


struct SeatSession {
    int id = -1;
    int seatId = -1;
    QString customerName;
    int guestCount = 1;
    QDateTime startTime;
    QDateTime endTime;
    QDateTime expectedEndTime; // For Internet Cafe mode
    SessionMode mode = SessionMode::Cafe;
    SessionStatus status = SessionStatus::Active;
    int prepaidMinutes = 0; // For Internet Cafe package
    int hourlyRate = 0; // cents per hour
    QString note;

    // Helper methods
    int getElapsedMinutes() const {
        if (!startTime.isValid()) return 0;
        QDateTime end = endTime.isValid() ? endTime : QDateTime::currentDateTime();
        return startTime.secsTo(end) / 60;
    }

    int getRemainingMinutes() const {
        if (!expectedEndTime.isValid() || prepaidMinutes == 0) return -1; // No limit
        QDateTime now = QDateTime::currentDateTime();
        int remaining = expectedEndTime.secsTo(now) / 60;
        return qMax(0, -remaining); // Negative means expired
    }

    bool isExpiringSoon(int thresholdMinutes = 10) const {
        int remaining = getRemainingMinutes();
        return remaining >= 0 && remaining <= thresholdMinutes;
    }

    bool isExpired() const {
        if (!expectedEndTime.isValid()) return false;
        return QDateTime::currentDateTime() > expectedEndTime;
    }
        
};