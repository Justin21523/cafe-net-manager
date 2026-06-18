#include <QApplication>
#include <QDir>
#include "core/AppContext.h"
#include "database/DatabaseManager.h"
#include "services/SeatService.h"
#include "database/SeatRepository.h"
#include "widgets/MainWindow.h"
#include "utils/Logger.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Logger::info("Application starting... V2.0 Architecture");

    AppContext *context = new AppContext();
    
    QString dbPath = QDir::currentPath() + "/data/cafenet.db";
    DatabaseManager *dbManager = context->databaseManager();
    
    if (!dbManager->initialize(dbPath)) {
        Logger::error("Critical: Database initialization failed.");
        return 1;
    }

    SeatService *seatService = context->seatService();

    std::vector<Seat> seats = seatService->loadAllSeats();

    MainWindow window;
    
    // 1. Inject Dependencies
    window.setDatabaseManager(dbManager);
    window.setServices(
        context->seatService(),
        context->seatSessionService(),
        context->menuService(),
        context->orderService(),
        context->seatRepository()
    );

    // 2. Initialize Pages (Must be after setServices)
    window.initPages(seats);
    
    // 3. Load Data & Show
    window.show();

    Logger::info("Application started successfully.");
    return app.exec();
}
