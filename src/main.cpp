#include <QApplication>
#include <QDir>
#include "core/AppContext.h"
#include "database/DatabaseManager.h"
#include "services/SeatService.h"
#include "services/SeatSessionService.h"
#include "services/MenuService.h"
#include "services/OrderService.h"
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
    SeatSessionService *sessionService = context->seatSessionService();
    MenuService *menuService = context->menuService();
    OrderService *orderService = context->orderService();
    SeatRepository *seatRepo = context->seatRepository();

    std::vector<Seat> seats = seatService->loadAllSeats();

    MainWindow window;
    window.setDatabaseManager(dbManager);
    window.setServices(seatService, sessionService, menuService, orderService, seatRepo);
    
    // Initialize Pages after dependencies are injected
    window.initPages(); 
    window.initializeSeatMap(seats);
    window.show();

    Logger::info("Application started successfully.");
    return app.exec();
}