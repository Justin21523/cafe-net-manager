#include "widgets/MainWindow.h"
#include "widgets/SeatMapView.h"
#include "widgets/SeatDetailPanel.h"

#include <QSplitter>
#include <QStatusBar>

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

    // Use QSplitter for resizable left/right panels
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    m_seatMapView = new SeatMapView(splitter);
    m_seatDetailPanel = new SeatDetailPanel(splitter);

    // Set initial sizes: Map takes 70%, Detail takes 30%
    splitter->addWidget(m_seatMapView);
    splitter->addWidget(m_seatDetailPanel);
    splitter->setSizes({800, 400});

    setCentralWidget(splitter);

    // Connect signals
    connect(m_seatMapView, &SeatMapView::seatSelected, 
            m_seatDetailPanel, &SeatDetailPanel::updateSeatInfo);

    statusBar()->showMessage("Ready");
}

void MainWindow::initializeSeatMap(const std::vector<Seat> &seats) {
    m_seatMapView->loadSeats(seats);
}