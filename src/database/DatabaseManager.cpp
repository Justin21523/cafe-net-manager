#include "database/DatabaseManager.h"
#include "utils/Logger.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QDir>
#include <QFileInfo>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent) {
    Logger::info("DatabaseManager initialized.");
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
    Logger::info("DatabaseManager destroyed.");
}

bool DatabaseManager::initialize(const QString &dbPath) {
    m_dbPath = dbPath;

    // 1. Ensure the directory exists
    QFileInfo fileInfo(m_dbPath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // 2. Setup Database Connection
    // We use a specific connection name to avoid conflicts with default connections
    if (QSqlDatabase::contains("cafenet_connection")) {
        m_db = QSqlDatabase::database("cafenet_connection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "cafenet_connection");
        m_db.setDatabaseName(m_dbPath);
    }

    if (!m_db.open()) {
        Logger::error("Failed to open database: " + m_db.lastError().text());
        return false;
    }

    Logger::info("Database opened successfully: " + m_dbPath);

    // 3. Initialize Schema and Seed Data
    if (!initializeSchema()) {
        Logger::error("Failed to initialize database schema.");
        return false;
    }

    if (!seedDemoData()) {
        Logger::error("Failed to seed demo data.");
        return false;
    }

    return true;
}

QSqlDatabase DatabaseManager::database() const {
    return m_db;
}

bool DatabaseManager::isOpen() const {
    return m_db.isOpen();
}

void DatabaseManager::closeDatabase() {
    if (m_db.isOpen()) {
        m_db.close();
        Logger::info("Database connection closed.");
    }
    
    // Remove the connection from Qt's connection pool
    if (QSqlDatabase::contains("cafenet_connection")) {
        QSqlDatabase::removeDatabase("cafenet_connection");
    }
}

bool DatabaseManager::initializeSchema() {
    QSqlQuery query(m_db);
    
    // Enable foreign key support in SQLite (disabled by default)
    if (!query.exec("PRAGMA foreign_keys = ON;")) {
        Logger::error("Failed to enable foreign keys: " + query.lastError().text());
        return false;
    }

    // SQL statements to create tables
    // We use CREATE TABLE IF NOT EXISTS to make it safe to run multiple times
    QStringList sqlStatements = {
        // Seats table
        R"(CREATE TABLE IF NOT EXISTS seats (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            code TEXT UNIQUE NOT NULL,
            name TEXT NOT NULL,
            area TEXT DEFAULT 'Main',
            type TEXT DEFAULT 'Table',
            x INTEGER DEFAULT 0,
            y INTEGER DEFAULT 0,
            width INTEGER DEFAULT 100,
            height INTEGER DEFAULT 100,
            status TEXT DEFAULT 'Available',
            capacity INTEGER DEFAULT 2,
            has_power_outlet INTEGER DEFAULT 0,
            is_quiet_zone INTEGER DEFAULT 0,
            note TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        ))",

        // Menu Categories table
        R"(CREATE TABLE IF NOT EXISTS menu_categories (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL,
            sort_order INTEGER DEFAULT 0,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        ))",

        // Menu Items table
        R"(CREATE TABLE IF NOT EXISTS menu_items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            category_id INTEGER,
            name TEXT NOT NULL,
            description TEXT,
            price INTEGER NOT NULL, -- Stored in cents to avoid floating point issues
            cost INTEGER DEFAULT 0,
            is_available INTEGER DEFAULT 1,
            is_active INTEGER DEFAULT 1,
            sort_order INTEGER DEFAULT 0,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (category_id) REFERENCES menu_categories(id) ON DELETE SET NULL
        ))",

        // Seat Sessions table
        R"(CREATE TABLE IF NOT EXISTS seat_sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            seat_id INTEGER NOT NULL,
            customer_name TEXT,
            guest_count INTEGER DEFAULT 1,
            start_time DATETIME NOT NULL,
            end_time DATETIME,
            mode TEXT DEFAULT 'Cafe', -- Cafe, InternetCafe, Study
            hourly_rate INTEGER DEFAULT 0,
            minimum_charge INTEGER DEFAULT 0,
            status TEXT DEFAULT 'Active',
            note TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (seat_id) REFERENCES seats(id) ON DELETE CASCADE
        ))",

        // Orders table
        R"(CREATE TABLE IF NOT EXISTS orders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            session_id INTEGER,
            seat_id INTEGER NOT NULL,
            order_number TEXT UNIQUE NOT NULL,
            status TEXT DEFAULT 'Draft',
            subtotal INTEGER DEFAULT 0,
            discount INTEGER DEFAULT 0,
            service_charge INTEGER DEFAULT 0,
            total INTEGER DEFAULT 0,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (session_id) REFERENCES seat_sessions(id) ON DELETE SET NULL,
            FOREIGN KEY (seat_id) REFERENCES seats(id) ON DELETE CASCADE
        ))",

        // Order Items table
        R"(CREATE TABLE IF NOT EXISTS order_items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            order_id INTEGER NOT NULL,
            menu_item_id INTEGER,
            item_name TEXT NOT NULL,
            quantity INTEGER NOT NULL DEFAULT 1,
            unit_price INTEGER NOT NULL,
            subtotal INTEGER NOT NULL,
            note TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (order_id) REFERENCES orders(id) ON DELETE CASCADE,
            FOREIGN KEY (menu_item_id) REFERENCES menu_items(id) ON DELETE SET NULL
        ))",

        // Payments table
        R"(CREATE TABLE IF NOT EXISTS payments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            order_id INTEGER NOT NULL,
            amount INTEGER NOT NULL,
            method TEXT NOT NULL, -- Cash, CreditCard, MemberBalance
            status TEXT DEFAULT 'Completed',
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (order_id) REFERENCES orders(id) ON DELETE CASCADE
        ))"
    };

    // Execute each statement
    for (const QString &sql : sqlStatements) {
        if (!query.exec(sql)) {
            Logger::error("Schema creation failed: " + query.lastError().text());
            Logger::error("SQL: " + sql);
            return false;
        }
    }

    Logger::info("Database schema initialized successfully.");
    return true;
}

bool DatabaseManager::seedDemoData() {
    QSqlQuery query(m_db);

    // Check if seats table is empty to avoid duplicate seeding
    query.exec("SELECT COUNT(*) FROM seats");
    if (query.next() && query.value(0).toInt() > 0) {
        Logger::info("Demo data already exists. Skipping seed.");
        return true;
    }

    Logger::info("Seeding demo data...");

    // Seed Menu Categories
    QStringList categories = {"Coffee", "Tea", "Dessert", "Main Course", "Internet Cafe"};
    for (int i = 0; i < categories.size(); ++i) {
        query.prepare("INSERT INTO menu_categories (name, sort_order) VALUES (:name, :order)");
        query.bindValue(":name", categories[i]);
        query.bindValue(":order", i);
        if (!query.exec()) {
            Logger::error("Failed to insert category: " + query.lastError().text());
            return false;
        }
    }

    // Seed Menu Items (Prices are in cents, e.g., 12000 = $120.00)
    // We assume category IDs are 1 to 5 based on insertion order
    struct MenuItemData {
        int categoryId;
        QString name;
        int price;
    };

    QList<MenuItemData> items = {
        {1, "Espresso", 8000},
        {1, "Cappuccino", 12000},
        {1, "Latte", 13000},
        {2, "Green Tea", 10000},
        {2, "Black Tea", 10000},
        {3, "Cheesecake", 15000},
        {3, "Chocolate Muffin", 12000},
        {4, "Pasta", 22000},
        {4, "Steak", 35000},
        {5, "1 Hour Internet", 5000},
        {5, "3 Hours Internet", 12000}
    };

    for (const auto &item : items) {
        query.prepare("INSERT INTO menu_items (category_id, name, price) VALUES (:cat_id, :name, :price)");
        query.bindValue(":cat_id", item.categoryId);
        query.bindValue(":name", item.name);
        query.bindValue(":price", item.price);
        if (!query.exec()) {
            Logger::error("Failed to insert menu item: " + query.lastError().text());
            return false;
        }
    }

    // Seed Seats
    QList<QString> seatCodes = {"A1", "A2", "A3", "B1", "B2", "C1"};
    for (int i = 0; i < seatCodes.size(); ++i) {
        query.prepare("INSERT INTO seats (code, name, area, capacity, x, y) VALUES (:code, :name, :area, :cap, :x, :y)");
        query.bindValue(":code", seatCodes[i]);
        query.bindValue(":name", "Seat " + seatCodes[i]);
        query.bindValue(":area", "Main Hall");
        query.bindValue(":cap", 2);
        query.bindValue(":x", (i % 3) * 120);
        query.bindValue(":y", (i / 3) * 120);
        if (!query.exec()) {
            Logger::error("Failed to insert seat: " + query.lastError().text());
            return false;
        }
    }

    Logger::info("Demo data seeded successfully.");
    return true;
}