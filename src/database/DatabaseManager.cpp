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
    query.exec("PRAGMA user_version;");
    int currentVersion = 0;
    if (query.next()) currentVersion = query.value(0).toInt();

    Logger::info("Current DB Schema Version: " + QString::number(currentVersion));

    // V1: Base Tables (Only run if version is 0)
    if (currentVersion < 1) {    
        // SQL statements to create tables
        // We use CREATE TABLE IF NOT EXISTS to make it safe to run multiple times
        QStringList v1Tables = {
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
                customer_id INTEGER,
                customer_name TEXT,
                guest_count INTEGER DEFAULT 1,
                start_time DATETIME NOT NULL,
                end_time DATETIME,
                expected_end_time DATETIME,
                mode TEXT DEFAULT 'Cafe',
                prepaid_minutes INTEGER DEFAULT 0,
                hourly_rate INTEGER DEFAULT 0,
                status TEXT DEFAULT 'Active',
                note TEXT,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (seat_id) REFERENCES seats(id) ON DELETE CASCADE,
                FOREIGN KEY (customer_id) REFERENCES customers(id) ON DELETE SET NULL
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
            ))",

            // Customer table
            R"(CREATE TABLE IF NOT EXISTS customers (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                phone TEXT,
                email TEXT,
                notes TEXT,
                total_visits INTEGER DEFAULT 0,
                total_spent INTEGER DEFAULT 0,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                last_visit DATETIME
            ))",
        };

        // Execute each statement
        for (const QString &sql : v1Tables) {
            if (!query.exec(sql)) {
                Logger::error("Schema creation failed: " + query.lastError().text());
                Logger::error("SQL: " + sql);
                return false;
            }
        }
        query.exec("PRAGMA user_version = 1;");
        Logger::info("Migrated to Schema V1.");
        currentVersion = 1;
    }
    // V2: Phase 3 Deep Modules (Inventory, Modifiers, Audit)
    if (currentVersion < 2) {
        if (!migrateToV2()) return false;
    }    

    return true;
}

bool DatabaseManager::seedDemoData() {
    QSqlQuery query(m_db);
    
    // Only seed if seats table is empty
    query.exec("SELECT COUNT(*) FROM seats");
    if (query.next() && query.value(0).toInt() > 0) return true;

    Logger::info("Seeding demo data...");

    // Seed categories
    QStringList cats = {"Coffee", "Tea", "Dessert", "Snack", "Beverage"};
    for (int i = 0; i < cats.size(); ++i) {
        query.prepare("INSERT INTO menu_categories (name, sort_order) VALUES (:name, :sort)");
        query.bindValue(":name", cats[i]);
        query.bindValue(":sort", i);
        query.exec();
    }

    // Seed menu items
    struct ItemSeed { QString cat; QString name; int price; };
    std::vector<ItemSeed> items = {
        {"Coffee", "Espresso", 350}, {"Coffee", "Latte", 450}, {"Coffee", "Cappuccino", 450},
        {"Coffee", "Americano", 350}, {"Coffee", "Mocha", 500},
        {"Tea", "Green Tea", 300}, {"Tea", "Earl Grey", 300}, {"Tea", "Matcha Latte", 480},
        {"Dessert", "Cheesecake", 550}, {"Dessert", "Tiramisu", 580},
        {"Snack", "Fries", 350}, {"Snack", "Toast", 280},
        {"Beverage", "Orange Juice", 380}, {"Beverage", "Sparkling Water", 200}
    };
    for (const auto &item : items) {
        query.prepare("INSERT INTO menu_items (category_id, name, price, is_available, is_active, cost) "
                      "SELECT id, :name, :price, 1, 1, :cost FROM menu_categories WHERE name = :cat");
        query.bindValue(":name", item.name);
        query.bindValue(":price", item.price);
        query.bindValue(":cost", item.price / 3);
        query.bindValue(":cat", item.cat);
        query.exec();
    }

    // Seed walls (environment objects are stored as seats with type='wall' for simplicity,
    // or we just create them in code. For DB persistence, let's use the seats table creatively)
    // Actually, walls are QGraphicsItems, not DB-backed in the same way.
    // Let's seed a rich set of seats instead.

    struct SeatSeed {
        QString code, name, area, type;
        int x, y, w, h, cap;
        bool power, quiet;
    };

    std::vector<SeatSeed> seats = {
        // Bar counter seats (top row)
        {"B1", "Bar 1", "Bar", "BarSeat", 100, 60, 50, 50, 1, false, false},
        {"B2", "Bar 2", "Bar", "BarSeat", 160, 60, 50, 50, 1, false, false},
        {"B3", "Bar 3", "Bar", "BarSeat", 220, 60, 50, 50, 1, false, false},
        {"B4", "Bar 4", "Bar", "BarSeat", 280, 60, 50, 50, 1, false, false},
        {"B5", "Bar 5", "Bar", "BarSeat", 340, 60, 50, 50, 1, true, false},
        {"B6", "Bar 6", "Bar", "BarSeat", 400, 60, 50, 50, 1, true, false},

        // Booth seats (left wall)
        {"L1", "Booth A", "Lounge", "Booth", 40, 180, 100, 80, 4, true, true},
        {"L2", "Booth B", "Lounge", "Booth", 40, 300, 100, 80, 4, true, true},
        {"L3", "Booth C", "Lounge", "Booth", 40, 420, 100, 80, 4, true, true},

        // Center tables (2x3 grid)
        {"C1", "Table 1", "Main", "SquareTable", 220, 200, 80, 80, 4, false, false},
        {"C2", "Table 2", "Main", "SquareTable", 340, 200, 80, 80, 4, false, false},
        {"C3", "Table 3", "Main", "SquareTable", 460, 200, 80, 80, 4, false, false},
        {"C4", "Table 4", "Main", "SquareTable", 220, 340, 80, 80, 4, true, false},
        {"C5", "Table 5", "Main", "SquareTable", 340, 340, 80, 80, 4, true, false},
        {"C6", "Table 6", "Main", "SquareTable", 460, 340, 80, 80, 4, true, false},

        // Window seats (right side)
        {"W1", "Window 1", "Window", "CoupleSeat", 620, 180, 60, 60, 2, true, true},
        {"W2", "Window 2", "Window", "CoupleSeat", 620, 280, 60, 60, 2, true, true},
        {"W3", "Window 3", "Window", "CoupleSeat", 620, 380, 60, 60, 2, true, true},

        // Study desks (bottom row)
        {"S1", "Study 1", "Study", "StudyDesk", 200, 500, 60, 50, 1, true, true},
        {"S2", "Study 2", "Study", "StudyDesk", 280, 500, 60, 50, 1, true, true},
        {"S3", "Study 3", "Study", "StudyDesk", 360, 500, 60, 50, 1, true, true},
        {"S4", "Study 4", "Study", "StudyDesk", 440, 500, 60, 50, 1, true, true},
        {"S5", "Study 5", "Study", "StudyDesk", 520, 500, 60, 50, 1, true, true},

        // Private room
        {"P1", "VIP Room", "Private", "PrivateRoom", 580, 460, 120, 100, 8, true, true},
    };

    for (const auto &s : seats) {
        query.prepare("INSERT INTO seats (code, name, area, type, x, y, width, height, status, capacity, has_power_outlet, is_quiet_zone) "
                      "VALUES (:code, :name, :area, :type, :x, :y, :w, :h, 'Available', :cap, :power, :quiet)");
        query.bindValue(":code", s.code);
        query.bindValue(":name", s.name);
        query.bindValue(":area", s.area);
        query.bindValue(":type", s.type);
        query.bindValue(":x", s.x);
        query.bindValue(":y", s.y);
        query.bindValue(":w", s.w);
        query.bindValue(":h", s.h);
        query.bindValue(":cap", s.cap);
        query.bindValue(":power", s.power ? 1 : 0);
        query.bindValue(":quiet", s.quiet ? 1 : 0);
        query.exec();
    }

    // Seed customers
    query.exec("INSERT OR IGNORE INTO customers (name, phone, email, total_visits, total_spent) VALUES ('Alice Wang', '0912345678', 'alice@test.com', 12, 35000)");
    query.exec("INSERT OR IGNORE INTO customers (name, phone, email, total_visits, total_spent) VALUES ('Bob Chen', '0987654321', 'bob@test.com', 5, 12000)");

    Logger::info(QString("Seeded %1 seats, %2 categories, %3 items.").arg(seats.size()).arg(cats.size()).arg(items.size()));
    return true;
}


bool DatabaseManager::migrateToV2() {
    QSqlQuery query(m_db);
    Logger::info("Starting Migration to V2 (Inventory, Modifiers, Audit)...");

    QStringList v2Tables = {
        // 3.2 Inventory & BOM
        "CREATE TABLE IF NOT EXISTS ingredients (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE, unit TEXT, current_stock REAL, safety_stock REAL, cost_per_unit INTEGER, created_at DATETIME DEFAULT CURRENT_TIMESTAMP)",
        "CREATE TABLE IF NOT EXISTS menu_item_ingredients (id INTEGER PRIMARY KEY AUTOINCREMENT, menu_item_id INTEGER, ingredient_id INTEGER, quantity REAL, FOREIGN KEY(menu_item_id) REFERENCES menu_items(id) ON DELETE CASCADE, FOREIGN KEY(ingredient_id) REFERENCES ingredients(id))",
        "CREATE TABLE IF NOT EXISTS stock_movements (id INTEGER PRIMARY KEY AUTOINCREMENT, ingredient_id INTEGER, quantity_change REAL, movement_type TEXT, reference_id INTEGER, created_at DATETIME DEFAULT CURRENT_TIMESTAMP, FOREIGN KEY(ingredient_id) REFERENCES ingredients(id))",
        
        // 3.3 Structured Modifiers
        "CREATE TABLE IF NOT EXISTS option_groups (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, is_required INTEGER)",
        "CREATE TABLE IF NOT EXISTS options (id INTEGER PRIMARY KEY AUTOINCREMENT, group_id INTEGER, name TEXT, price_modifier INTEGER, FOREIGN KEY(group_id) REFERENCES option_groups(id) ON DELETE CASCADE)",
        "CREATE TABLE IF NOT EXISTS menu_item_option_groups (id INTEGER PRIMARY KEY AUTOINCREMENT, menu_item_id INTEGER, group_id INTEGER, FOREIGN KEY(menu_item_id) REFERENCES menu_items(id) ON DELETE CASCADE, FOREIGN KEY(group_id) REFERENCES option_groups(id) ON DELETE CASCADE)",
        
        // 3.4 Employee & Audit
        "CREATE TABLE IF NOT EXISTS employees (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE, role TEXT, pin_code TEXT, is_active INTEGER, created_at DATETIME DEFAULT CURRENT_TIMESTAMP)",
        "CREATE TABLE IF NOT EXISTS audit_logs (id INTEGER PRIMARY KEY AUTOINCREMENT, employee_id INTEGER, action_type TEXT, target_table TEXT, target_id INTEGER, details TEXT, created_at DATETIME DEFAULT CURRENT_TIMESTAMP)"
    };

    for (const QString &sql : v2Tables) {
        if (!query.exec(sql)) {
            Logger::error("V2 Schema failed: " + query.lastError().text() + " | SQL: " + sql);
            return false;
        }
    }
    // Seed some V2 demo data
    query.exec("INSERT OR IGNORE INTO ingredients (name, unit, current_stock, safety_stock, cost_per_unit) VALUES ('Coffee Beans', 'g', 5000, 1000, 100)");
    query.exec("INSERT OR IGNORE INTO ingredients (name, unit, current_stock, safety_stock, cost_per_unit) VALUES ('Milk', 'ml', 10000, 2000, 50)");
    
    // Link Espresso to Coffee Beans
    query.exec("INSERT OR IGNORE INTO menu_item_ingredients (menu_item_id, ingredient_id, quantity) SELECT mi.id, i.id, 18.0 FROM menu_items mi, ingredients i WHERE mi.name = 'Espresso' AND i.name = 'Coffee Beans'");
    
    query.exec("INSERT OR IGNORE INTO employees (name, role, pin_code, is_active) VALUES ('Admin', 'Manager', '1234', 1)");
    query.exec("INSERT OR IGNORE INTO employees (name, role, pin_code, is_active) VALUES ('Justin', 'Staff', '5678', 1)");

    query.exec("PRAGMA user_version = 2;");
    Logger::info("Successfully migrated to Schema V2.");
    return true;
}
