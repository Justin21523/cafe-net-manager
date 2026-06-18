#include "widgets/MainWindow.h"
#include "widgets/SidebarWidget.h"
#include "pages/DashboardPage.h"
#include "pages/FloorPlanPage.h"
#include "pages/PosOrderPage.h"
#include "pages/KitchenPage.h"
#include "pages/AdminPage.h"

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

    // 1. Left Sidebar
    m_sidebar = new SidebarWidget(centralWidget);
    
    // 2. Central Stacked Widget for Pages
    m_stackedWidget = new QStackedWidget(centralWidget);
    
    mainLayout->addWidget(m_sidebar);
    mainLayout->addWidget(m_stackedWidget, 1); // Stretch factor 1

    setCentralWidget(centralWidget);
    
    // Connect Sidebar navigation to StackedWidget
    connect(m_sidebar, &SidebarWidget::navigateTo, this, &MainWindow::handleNavigateTo);
    
    statusBar()->showMessage("Ready - V2.0 Architecture Loaded");
}

void MainWindow::initPages(const std::vector<Seat> &seats) {
    // 1. Instantiate Pages
    m_dashboardPage = new DashboardPage(m_dbManager, m_inventoryService, m_stackedWidget);
    
    // 注入 SeatSessionService 和 OrderService 給 FloorPlanPage
    m_floorPlanPage = new FloorPlanPage(
        m_seatService,
        m_sessionService,
        m_orderService,
        m_stackedWidget
    );
    
    m_posOrderPage = new PosOrderPage(m_stackedWidget);
    m_kitchenPage = new KitchenPage(m_stackedWidget);
    m_adminPage = new AdminPage(m_dbManager, m_stackedWidget);

    if (m_posOrderPage) m_posOrderPage->init(m_menuService, m_orderService);
    if (m_kitchenPage) m_kitchenPage->init(m_orderService);

    // 2. Add to StackedWidget
    m_stackedWidget->addWidget(m_dashboardPage);   // 0
    m_stackedWidget->addWidget(m_floorPlanPage);   // 1
    m_stackedWidget->addWidget(m_posOrderPage);    // 2
    m_stackedWidget->addWidget(m_kitchenPage);     // 3
    m_stackedWidget->addWidget(m_adminPage);        // 4
    
    m_stackedWidget->setCurrentIndex(1);
    
    // 3. Wire Cross-Page Signals
    connect(m_floorPlanPage, &FloorPlanPage::sessionChanged, m_dashboardPage, &DashboardPage::refreshData);
    connect(m_posOrderPage, &PosOrderPage::orderSubmitted, m_kitchenPage, &KitchenPage::refreshBoard);
    
    // NEW: Handle navigation from Seat Operation Panel
    connect(m_floorPlanPage, &FloorPlanPage::checkoutRequested, this, &MainWindow::handleCheckoutFromSeat);
    
    connect(m_floorPlanPage, &FloorPlanPage::addOrderRequested, this, [this](int seatId, const QString &seatCode) {
        if (m_posOrderPage) {
            m_posOrderPage->setTargetSeat(seatId, seatCode);
            m_stackedWidget->setCurrentWidget(m_posOrderPage); // Switch to POS Page
        }
    });
    
    connect(m_sidebar, &SidebarWidget::navigateTo, this, [this](int index) {
        if (index == 2) { // POS Page index
            // 如果還沒設定座位，可以預設為 -1, "Walk-in"
            // 這取決於你的業務邏輯
        }
    });   
    // 6. Cross-Page Sync (Admin changes -> Refresh Frontend)
    connect(m_adminPage, &AdminPage::dataUpdated, this, [this](const QString &module) {
        if (module == "seats" && m_floorPlanPage) {
            // 假設 FloorPlanPage 有 refreshMap() 或重新載入的方法
            // 這裡我們用一個簡單的方式：重新初始化
            // 注意：你需要確保 FloorPlanPage 能重新載入，或者發射信號給它
            statusBar()->showMessage("Seats updated in Admin. Please refresh Floor Plan if needed.", 3000);
        } else if (module == "menu" && m_posOrderPage) {
            statusBar()->showMessage("Menu updated in Admin. POS page will refresh on next visit.", 3000);
        }
    });

    connectSignals();

    // 4. Initialize Data
    m_floorPlanPage->initializeSeatMap(seats);
}


void MainWindow::connectSignals() {
    // Floor Plan Signals
    connect(m_floorPlanPage, &FloorPlanPage::startSessionRequested, this, &MainWindow::handleStartSession);
    connect(m_floorPlanPage, &FloorPlanPage::endSessionRequested, this, &MainWindow::handleEndSession);
    connect(m_floorPlanPage, &FloorPlanPage::extendSessionRequested, this, &MainWindow::handleExtendSession);
    connect(m_floorPlanPage, &FloorPlanPage::seatPositionChanged, this, &MainWindow::handleSeatPositionChanged);
    
    connect(m_floorPlanPage, &FloorPlanPage::seatSelected, this, [this](const Seat &seat) {
        m_selectedSeatId = seat.id;
        if (m_posOrderPage) m_posOrderPage->setTargetSeat(seat.id, seat.code);
    });

    // POS Signals
    connect(m_posOrderPage, &PosOrderPage::orderSubmitted, this, &MainWindow::handleOrderSubmitted);
}

void MainWindow::setDatabaseManager(DatabaseManager *dbManager) { m_dbManager = dbManager; }

void MainWindow::setServices(SeatService *seatService, SeatSessionService *sessionService, 
                             MenuService *menuService, OrderService *orderService,
                             SeatRepository *seatRepository,
                             InventoryService *inventoryService) {
    m_seatService = seatService;
    m_sessionService = sessionService;
    m_menuService = menuService;
    m_orderService = orderService;
    m_seatRepository = seatRepository;
    m_inventoryService = inventoryService;
}

void MainWindow::initializeSeatMap(const std::vector<Seat> &seats) {
    if (m_floorPlanPage) m_floorPlanPage->initializeSeatMap(seats);
}

void MainWindow::handleNavigateTo(int index) {
    if (m_stackedWidget) {
        if (index < 0 || index >= m_stackedWidget->count()) {
            statusBar()->showMessage("Navigation target unavailable", 3000);
            return;
        }

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

void MainWindow::handleNavigateToPos(int seatId) {
    if (m_posOrderPage) {
        m_posOrderPage->setTargetSeat(seatId, QString::number(seatId));
        m_stackedWidget->setCurrentWidget(m_posOrderPage); // Switch to POS Page
        // 更新 Sidebar 狀態 (可選)
    }
}

void MainWindow::handleCheckoutFromSeat(int seatId) {
    // 這裡可以實作開啟 CheckoutDialog 的邏輯
    // 為了保持程式碼簡潔，我們先印出日誌，V2.3 或後續會完善結帳流程
    statusBar()->showMessage("Checkout requested for Seat ID: " + QString::number(seatId), 3000);
}
