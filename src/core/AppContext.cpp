#include "core/AppContext.h"
#include "database/DatabaseManager.h"
#include "database/SeatRepository.h"
#include "services/SeatService.h"
#include "utils/Logger.h"

AppContext::AppContext(QObject *parent)
    : QObject(parent) {
    m_databaseManager = new DatabaseManager(this);
    
    // Repository needs DatabaseManager
    m_seatRepository = new SeatRepository(m_databaseManager);
    
    // Service needs Repository
    m_seatService = new SeatService(m_seatRepository);

    Logger::info("AppContext initialized.");
}

AppContext::~AppContext() {
    Logger::info("AppContext destroyed.");
    // QObject parent-child ownership handles deletion automatically.
}

DatabaseManager* AppContext::databaseManager() const {
    return m_databaseManager;
}

SeatService* AppContext::seatService() const {
    return m_seatService;
}