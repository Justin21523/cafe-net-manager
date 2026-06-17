#include "widgets/MainWindow.h"
#include "widgets/SeatMapView.h"
#include "widgets/SeatDetailPanel.h"
#include "widgets/MenuBrowserWidget.h"
#include "services/SeatService.h"
#include "services/SeatSessionService.h"
#include "services/MenuService.h"
#include "models/MenuItem.h"
#include "widgets/CartWidget.h"
#include "services/OrderService.h"
#include "widgets/KitchenBoardWidget.h"
#include "widgets/CheckoutDialog.h"
#include "widgets/AdminPage.h"
#include "widgets/DashboardWidget.h"
#include "database/DatabaseManager.h"
#include "database/SeatRepository.h"
#include "utils/Logger.h"

#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setupUI();
}

MainWindow::~MainWindow() {
    // Child widgets will be deleted automatically by Qt's parent-child ownership.
}

void MainWindow::setupUI() {
    setWindowTitle("CafeNet Manager");
    resize(1200, 800);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    
    m_seatMapView = new SeatMapView(splitter);
    
    // Create Tab Widget for the right panel
    m_rightPanelTabs = new QTabWidget(splitter);
    m_seatDetailPanel = new SeatDetailPanel();
    m_menuBrowserWidget = nullptr; // Will be initialized in setServices

    m_rightPanelTabs->addTab(m_seatDetailPanel, "Seat Details");
    // Menu tab will be added in setServices

    splitter->addWidget(m_seatMapView);
    splitter->addWidget(m_rightPanelTabs);
    splitter->setSizes({800, 400});

    setCentralWidget(splitter);

    // CRITICAL: Connect seat selection to detail panel
    connect(m_seatMapView, &SeatMapView::seatSelected, 
            m_seatDetailPanel, &SeatDetailPanel::updateSeatInfo);
    
    // Connect seat selection to update current seat
    connect(m_seatMapView, &SeatMapView::seatSelected, 
            this, [this](const Seat &seat) {
                setSelectedSeat(seat.id, -1);
                Logger::info("Seat selected: " + seat.code + " (ID: " + QString::number(seat.id) + ")");
            });

    // Connect session buttons
    connect(m_seatDetailPanel, &SeatDetailPanel::startSessionRequested, 
            this, &MainWindow::handleStartSession);
    connect(m_seatDetailPanel, &SeatDetailPanel::endSessionRequested, 
            this, &MainWindow::handleEndSession);
    connect(m_seatDetailPanel, &SeatDetailPanel::extendSessionRequested, 
            this, &MainWindow::handleExtendSession);

    // Connect seat position changes
    connect(m_seatMapView, &SeatMapView::seatPositionChanged, 
            this, &MainWindow::handleSeatPositionChanged);

    statusBar()->showMessage("Ready - Select a seat to view details");
}

void MainWindow::setDatabaseManager(DatabaseManager *dbManager) {
    if (dbManager) {
        // Create Dashboard as first tab
        m_dashboardWidget = new DashboardWidget(dbManager, m_rightPanelTabs);
        m_rightPanelTabs->insertTab(0, m_dashboardWidget, "Dashboard");
    }
}

void MainWindow::initializeSeatMap(const std::vector<Seat> &seats) {
    m_seatMapView->loadSeats(seats);
}

// ... add setServices ...
void MainWindow::setServices(SeatService *seatService, SeatSessionService *sessionService, 
                             MenuService *menuService, OrderService *orderService,
                             SeatRepository *seatRepository) {
    m_seatService = seatService;
    m_sessionService = sessionService;
    m_orderService = orderService;
    m_seatRepository = seatRepository; // Store it

    connect(m_seatDetailPanel, &SeatDetailPanel::startSessionRequested, this, &MainWindow::handleStartSession);
    connect(m_seatDetailPanel, &SeatDetailPanel::endSessionRequested, this, &MainWindow::handleEndSession);

    // Menu tab
    if (menuService) {
        m_menuBrowserWidget = new MenuBrowserWidget(menuService, m_rightPanelTabs);
        m_rightPanelTabs->addTab(m_menuBrowserWidget, "Menu");
        
        connect(m_menuBrowserWidget, &MenuBrowserWidget::itemAddedToCart, 
                this, &MainWindow::handleItemAddedToCart);
    }

    // Cart tab
    if (orderService) {
        m_cartWidget = new CartWidget(orderService, m_rightPanelTabs);
        m_rightPanelTabs->addTab(m_cartWidget, "Cart");
        
        connect(m_cartWidget, &CartWidget::orderSubmitted, this, [this]() {
            statusBar()->showMessage("Order submitted successfully!", 3000);
            if (m_kitchenBoardWidget) {
                m_kitchenBoardWidget->refreshBoard();
            }
            if (m_dashboardWidget) {
                m_dashboardWidget->refreshData();
            }
        });

        // Kitchen Board tab
        m_kitchenBoardWidget = new KitchenBoardWidget(orderService, m_rightPanelTabs);
        m_rightPanelTabs->addTab(m_kitchenBoardWidget, "Kitchen Board");
    }
}

void MainWindow::setSelectedSeat(int seatId, int sessionId) {
    m_selectedSeatId = seatId;
    m_selectedSessionId = sessionId;
    if (m_cartWidget) {
        m_cartWidget->setSelectedSeat(seatId, sessionId);
    }
    Logger::info("Selected seat ID: " + QString::number(seatId) + ", Session ID: " + QString::number(sessionId));
}

void MainWindow::handleStartSession(int seatId) {
    Logger::info("MainWindow: Starting session for seat " + QString::number(seatId));
    if (m_sessionService) {
        if (m_sessionService->startSession(seatId)) {
            statusBar()->showMessage("Session started for seat", 3000);
            refreshSeatMap();
            if (m_dashboardWidget) {
                m_dashboardWidget->refreshData();
            }
        } else {
            statusBar()->showMessage("Failed to start session", 3000);
        }
    }
}


void MainWindow::handleExtendSession(int sessionId, int minutes) {
    if (m_sessionService) {
        if (m_sessionService->extendSession(sessionId, minutes)) {
            statusBar()->showMessage(QString("Session extended by %1 minutes").arg(minutes), 3000);
            refreshSeatMap();
        }
    }
}

void MainWindow::handleEndSession(int seatId) {
    Logger::info("MainWindow: Ending session for seat " + QString::number(seatId));
    if (m_sessionService) {
        if (m_sessionService->endSession(seatId)) {
            statusBar()->showMessage("Session ended", 3000);
            refreshSeatMap();
            if (m_dashboardWidget) {
                m_dashboardWidget->refreshData();
            }
        } else {
            statusBar()->showMessage("Failed to end session", 3000);
        }
    }
}

void MainWindow::refreshSeatMap() {
    if (m_seatService) {
        std::vector<Seat> seats = m_seatService->loadAllSeats();
        m_seatMapView->loadSeats(seats);
        m_seatDetailPanel->clearSelection();
        statusBar()->showMessage("Seat map refreshed.", 3000);
    }
}

void MainWindow::handleItemAddedToCart(const MenuItem &item) {
    if (m_orderService) {
        m_orderService->addToCart(item);
        if (m_cartWidget) {
            m_cartWidget->refreshCart();
        }
        statusBar()->showMessage("Added to cart: " + item.name, 3000);
    } else {
        statusBar()->showMessage("Error: Order service not available", 3000);
    }

}

void MainWindow::handleCheckout(const Order &order) {
    CheckoutDialog dialog(order, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Process payment
        statusBar()->showMessage("Payment completed!", 3000);
    }
}

// 在 MainWindow 中加入選單或按鈕來開啟 Admin Page
void MainWindow::showAdminPage() {
    if (!m_adminPage) {
        m_adminPage = new AdminPage(this);
        // 這裡可以設定為獨立視窗或新的分頁
    }
    m_adminPage->show();
}

void MainWindow::handleSeatPositionChanged(int seatId, int x, int y) {
    if (m_seatRepository) {
        m_seatRepository->updateSeatPosition(seatId, x, y);
    }
}
