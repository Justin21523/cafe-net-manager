#pragma once

#include <QString>

struct MenuItem {
    int id = -1;
    int categoryId = -1;
    QString name;
    QString description;
    int price = 0; // Stored in cents
    bool isAvailable = true;
};