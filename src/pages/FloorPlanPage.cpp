#include "pages/FloorPlanPage.h"
#include "widgets/SeatMapView.h"
#include "widgets/SeatDetailPanel.h"
#include <QSplitter>
#include <QVBoxLayout>

FloorPlanPage::FloorPlanPage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    
    m_seatMapView = new SeatMapView(splitter);
    m_seatDetailPanel = new SeatDetailPanel(splitter);
    
    splitter->addWidget(m_seatMapView);
    splitter->addWidget(m_seatDetailPanel);
    splitter->setSizes({900, 400});
    
    layout->addWidget(splitter);

    // Internal signal connections
    connect(m_seatMapView, &SeatMapView::seatSelected, m_seatDetailPanel, &SeatDetailPanel::updateSeatInfo);
    
    // Forward signals to MainWindow
    connect(m_seatMapView, &SeatMapView::seatSelected, this, &FloorPlanPage::seatSelected);
    connect(m_seatMapView, &SeatMapView::seatPositionChanged, this, &FloorPlanPage::seatPositionChanged);
    
    connect(m_seatDetailPanel, &SeatDetailPanel::startSessionRequested, this, &FloorPlanPage::startSessionRequested);
    connect(m_seatDetailPanel, &SeatDetailPanel::endSessionRequested, this, &FloorPlanPage::endSessionRequested);
    connect(m_seatDetailPanel, &SeatDetailPanel::extendSessionRequested, this, &FloorPlanPage::extendSessionRequested);
}

void FloorPlanPage::initializeSeatMap(const std::vector<Seat> &seats) {
    m_seatMapView->loadSeats(seats);
}