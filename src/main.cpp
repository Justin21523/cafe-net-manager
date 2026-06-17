#include <QApplication>
#include <QDir>
#include "core/AppContext.h"
#include "database/DatabaseManager.h"
#include "services/SeatService.h"
#include "models/Seat.h"
#include "widgets/MainWindow.h"
#include "utils/Logger.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Logger::info("Application starting...");

    AppContext *context = new AppContext();
    
    QString dbPath = QDir::currentPath() + "/data/cafenet.db";
    DatabaseManager *dbManager = context->databaseManager();
    
    if (!dbManager->initialize(dbPath)) {
        Logger::error("Critical: Database initialization failed. Exiting.");
        return 1;
    }

    SeatService *seatService = context->seatService();
    SeatSessionService *sessionService = context->seatSessionService();
    MenuService *menuService = context->menuService(); // Added

    std::vector<Seat> seats = seatService->loadAllSeats();

    MainWindow window;
    OrderService *orderService = context->orderService();
    window.setDatabaseManager(dbManager);  // Add this line
    SeatRepository *seatRepo = context->seatRepository(); 
    
    window.setServices(seatService, sessionService, menuService, orderService, seatRepo);
    window.initializeSeatMap(seats);
    window.show();

    Logger::info("Application started successfully.");

    return app.exec();
}