#include "core/AppContext.h"
#include "database/DatabaseManager.h"
#include "database/SeatRepository.h"
#include "database/SeatSessionRepository.h"
#include "services/SeatService.h"
#include "services/SeatSessionService.h"
#include "database/MenuRepository.h"
#include "services/MenuService.h"
#include "database/OrderRepository.h"
#include "services/OrderService.h"
#include "utils/Logger.h"

AppContext::AppContext(QObject *parent)
    : QObject(parent) {
    m_databaseManager = new DatabaseManager(this);
    
    // Repository needs DatabaseManager
    m_seatRepository = new SeatRepository(m_databaseManager);
    // Service needs Repository
    m_seatService = new SeatService(m_seatRepository);
    
    m_sessionRepository = new SeatSessionRepository(m_databaseManager);
    m_sessionService = new SeatSessionService(m_seatRepository, m_sessionRepository);

    m_menuRepository = new MenuRepository(m_databaseManager);
    m_menuService = new MenuService(m_menuRepository);

    m_orderRepository = new OrderRepository(m_databaseManager);
    m_orderService = new OrderService(m_orderRepository);

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

SeatSessionService* AppContext::seatSessionService() const {
    return m_sessionService;
}

MenuService* AppContext::menuService() const {
    return m_menuService;
}

OrderService* AppContext::orderService() const {
    return m_orderService;
}