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

    connect(m_seatMapView, &SeatMapView::seatSelected, 
            m_seatDetailPanel, &SeatDetailPanel::updateSeatInfo);

    statusBar()->showMessage("Ready");
}

void MainWindow::initializeSeatMap(const std::vector<Seat> &seats) {
    m_seatMapView->loadSeats(seats);
}

// ... add setServices ...
void MainWindow::setServices(SeatService *seatService, SeatSessionService *sessionService, 
                             MenuService *menuService, OrderService *orderService) {
    m_seatService = seatService;
    m_sessionService = sessionService;
    m_orderService = orderService;

    connect(m_seatDetailPanel, &SeatDetailPanel::startSessionRequested, this, &MainWindow::handleStartSession);
    connect(m_seatDetailPanel, &SeatDetailPanel::endSessionRequested, this, &MainWindow::handleEndSession);

    if (menuService) {
        m_menuBrowserWidget = new MenuBrowserWidget(menuService, m_rightPanelTabs);
        m_rightPanelTabs->addTab(m_menuBrowserWidget, "Menu");
        
        connect(m_menuBrowserWidget, &MenuBrowserWidget::itemAddedToCart, 
                this, &MainWindow::handleItemAddedToCart);
    }

    if (orderService) {
        m_cartWidget = new CartWidget(orderService, m_rightPanelTabs);
        m_rightPanelTabs->addTab(m_cartWidget, "Cart");
        
        connect(m_cartWidget, &CartWidget::orderSubmitted, this, [this]() {
            statusBar()->showMessage("Order submitted successfully!", 3000);
        });
    }
}

void MainWindow::setSelectedSeat(int seatId, int sessionId) {
    m_selectedSeatId = seatId;
    m_selectedSessionId = sessionId;
    if (m_cartWidget) {
        // Pass seat info to cart widget if needed
    }
}

void MainWindow::handleStartSession(int seatId) {
    if (m_sessionService && m_sessionService->startSession(seatId)) {
        refreshSeatMap();
    }
}

void MainWindow::handleEndSession(int seatId) {
    if (m_sessionService && m_sessionService->endSession(seatId)) {
        refreshSeatMap();
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
    }
}
