#pragma once

#include <QString>
#include <QDateTime>

struct Customer {
    int id = -1;
    QString name;
    QString phone;
    QString email;
    QString notes;
    int totalVisits = 0;
    int totalSpent = 0; // cents
    QDateTime createdAt;
    QDateTime lastVisit;
};