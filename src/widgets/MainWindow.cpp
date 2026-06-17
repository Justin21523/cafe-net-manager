#include "widgets/MainWindow.h"
#include "widgets/SidebarWidget.h"
#include "pages/DashboardPage.h"
#include "pages/FloorPlanPage.h"
#include "pages/PosOrderPage.h"
#include "pages/KitchenPage.h"

#include "services/SeatService.h"
#include "services/SeatSessionService.h"
#include "services/MenuService.h"
#include "services/OrderService.h"
#include "database/DatabaseManager.h"
#include "database/SeatRepository.h"

#include <QStackedWidget>
#include <QHBoxLayout>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    setWindowTitle("CafeNet Manager V2.0");
    resize(1400, 900);

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_sidebar = new SidebarWidget(centralWidget);
    m_stackedWidget = new QStackedWidget(centralWidget);
    
    mainLayout->addWidget(m_sidebar);
    mainLayout->addWidget(m_stackedWidget, 1); // Stretch factor 1

    setCentralWidget(centralWidget);
    
    connect(m_sidebar, &SidebarWidget::navigateTo, this, &MainWindow::handleNavigateTo);
    
    statusBar()->showMessage("Ready - V2.0 Architecture Loaded");
}

void MainWindow::initPages() {
    // Create Pages with injected dependencies
    m_dashboardPage = new DashboardPage(m_dbManager, m_stackedWidget);
    m_floorPlanPage = new FloorPlanPage(m_stackedWidget);
    m_posOrderPage = new PosOrderPage(m_stackedWidget);
    m_kitchenPage = new KitchenPage(m_stackedWidget);
    
    // Initialize pages that need services
    if (m_posOrderPage) m_posOrderPage->init(m_menuService, m_orderService);
    if (m_kitchenPage) m_kitchenPage->init(m_orderService);
    
    // Add to StackedWidget
    m_stackedWidget->addWidget(m_dashboardPage); // Index 0
    m_stackedWidget->addWidget(m_floorPlanPage); // Index 1
    m_stackedWidget->addWidget(m_posOrderPage);  // Index 2
    m_stackedWidget->addWidget(m_kitchenPage);   // Index 3
    
    // Default to Floor Plan
    m_stackedWidget->setCurrentIndex(1);
    
    connectSignals();
}

void MainWindow::connectSignals() {
    // Floor Plan Signals
    connect(m_floorPlanPage, &FloorPlanPage::startSessionRequested, this, &MainWindow::handleStartSession);
    connect(m_floorPlanPage, &FloorPlanPage::endSessionRequested, this, &MainWindow::handleEndSession);
    connect(m_floorPlanPage, &FloorPlanPage::extendSessionRequested, this, &MainWindow::handleExtendSession);
    connect(m_floorPlanPage, &FloorPlanPage::seatPositionChanged, this, &MainWindow::handleSeatPositionChanged);
    
    connect(m_floorPlanPage, &FloorPlanPage::seatSelected, this, [this](const Seat &seat) {
        m_selectedSeatId = seat.id;
        if (m_posOrderPage) m_posOrderPage->setSelectedSeat(seat.id, -1);
    });

    // POS Signals
    connect(m_posOrderPage, &PosOrderPage::itemAddedToCart, this, &MainWindow::handleItemAddedToCart);
    connect(m_posOrderPage, &PosOrderPage::orderSubmitted, this, &MainWindow::handleOrderSubmitted);
}

void MainWindow::setDatabaseManager(DatabaseManager *dbManager) { m_dbManager = dbManager; }

void MainWindow::setServices(SeatService *seatService, SeatSessionService *sessionService, 
                             MenuService *menuService, OrderService *orderService,
                             SeatRepository *seatRepository) {
    m_seatService = seatService;
    m_sessionService = sessionService;
    m_menuService = menuService;
    m_orderService = orderService;
    m_seatRepository = seatRepository;
}

void MainWindow::initializeSeatMap(const std::vector<Seat> &seats) {
    if (m_floorPlanPage) m_floorPlanPage->initializeSeatMap(seats);
}

void MainWindow::handleNavigateTo(int index) {
    if (m_stackedWidget) {
        m_stackedWidget->setCurrentIndex(index);
        // Refresh data when navigating to specific pages
        if (index == 0 && m_dashboardPage) m_dashboardPage->refreshData();
        if (index == 3 && m_kitchenPage) m_kitchenPage->refreshBoard();
    }
}

void MainWindow::handleStartSession(int seatId) {
    if (m_sessionService && m_sessionService->startSession(seatId)) {
        statusBar()->showMessage("Session started", 3000);
        initializeSeatMap(m_seatService->loadAllSeats());
        if (m_dashboardPage) m_dashboardPage->refreshData();
    }
}

void MainWindow::handleEndSession(int seatId) {
    if (m_sessionService && m_sessionService->endSession(seatId)) {
        statusBar()->showMessage("Session ended", 3000);
        initializeSeatMap(m_seatService->loadAllSeats());
        if (m_dashboardPage) m_dashboardPage->refreshData();
    }
}

void MainWindow::handleExtendSession(int sessionId, int minutes) {
    if (m_sessionService && m_sessionService->extendSession(sessionId, minutes)) {
        statusBar()->showMessage(QString("Extended by %1 mins").arg(minutes), 3000);
    }
}

void MainWindow::handleSeatPositionChanged(int seatId, int x, int y) {
    if (m_seatRepository) m_seatRepository->updateSeatPosition(seatId, x, y);
}

void MainWindow::handleItemAddedToCart(const MenuItem &item) {
    if (m_orderService) {
        m_orderService->addToCart(item);
        statusBar()->showMessage("Added: " + item.name, 2000);
    }
}

void MainWindow::handleOrderSubmitted() {
    statusBar()->showMessage("Order submitted to kitchen!", 3000);
    if (m_kitchenPage) m_kitchenPage->refreshBoard();
    if (m_dashboardPage) m_dashboardPage->refreshData();
}