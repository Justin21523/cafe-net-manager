#include "widgets/SeatMapView.h"
#include "widgets/SeatGraphicsItem.h"
#include "utils/Logger.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>

SeatMapView::SeatMapView(QWidget *parent)
    : QWidget(parent) {
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, 800, 600);

    m_view = new QGraphicsView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    // In Edit Mode, we don't want rubber band drag to interfere with item drag
    m_view->setDragMode(QGraphicsView::NoDrag); 

    // Control Panel
    QHBoxLayout *controlLayout = new QHBoxLayout();
    m_editToggleBtn = new QPushButton("Enable Edit Mode", this);
    m_saveBtn = new QPushButton("Save Layout", this);
    m_saveBtn->setEnabled(false);
    
    controlLayout->addWidget(m_editToggleBtn);
    controlLayout->addWidget(m_saveBtn);
    controlLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(m_view);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Connections
    connect(m_editToggleBtn, &QPushButton::clicked, this, [this]() {
        m_isEditMode = !m_isEditMode;
        setEditMode(m_isEditMode);
    });

    connect(m_saveBtn, &QPushButton::clicked, this, &SeatMapView::saveLayout);
}

SeatMapView::~SeatMapView() {
    // QGraphicsScene will automatically delete its items when destroyed.
}

void SeatMapView::loadSeats(const std::vector<Seat> &seats) {
    m_scene->clear();
    m_items.clear();

    for (const auto &seat : seats) {
        SeatGraphicsItem *item = new SeatGraphicsItem(seat);
        item->setEditMode(m_isEditMode); // Apply current mode
        m_scene->addItem(item);
        m_items.push_back(item);
    }
}

void SeatMapView::setEditMode(bool enabled) {
    m_isEditMode = enabled;
    
    if (m_isEditMode) {
        m_editToggleBtn->setText("Disable Edit Mode");
        m_editToggleBtn->setStyleSheet("background-color: #ff9800; color: white;");
        m_saveBtn->setEnabled(true);
        
        // Clear selection when entering edit mode
        m_scene->clearSelection();
    } else {
        m_editToggleBtn->setText("Enable Edit Mode");
        m_editToggleBtn->setStyleSheet("");
        m_saveBtn->setEnabled(false);
    }

    // Update all items
    for (SeatGraphicsItem *item : m_items) {
        item->setEditMode(enabled);
    }
}

void SeatMapView::saveLayout() {
    // Note: In a real app, we would pass a SeatRepository pointer to SeatMapView.
    // For simplicity here, we will emit a signal or use a callback. 
    // Let's add a signal for saving.
    
    for (SeatGraphicsItem *item : m_items) {
        const Seat &seatData = item->seatData();
        // QGraphicsItem::pos() returns the position relative to parent. 
        // Since we added them directly to scene, pos() is scene coordinates.
        QPointF pos = item->pos();
        
        // Emit signal to save (we will connect this in MainWindow)
        emit seatPositionChanged(seatData.id, static_cast<int>(pos.x()), static_cast<int>(pos.y()));
    }
    
    Logger::info("Layout saved to database.");
    // Optional: Switch back to view mode after saving
    // setEditMode(false); 
}