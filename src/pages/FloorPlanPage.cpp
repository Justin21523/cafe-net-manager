#include "pages/FloorPlanPage.h"
#include "graphics/FloorPlanScene.h"
#include "widgets/FloorPlanView.h"
#include "widgets/ToolPalette.h"
#include "widgets/InspectorPanel.h"
#include "graphics/SeatItem.h"
#include "graphics/FloorPlanItem.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>

FloorPlanPage::FloorPlanPage(QWidget *parent) : QWidget(parent) {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 1. Left: Tool Palette
    m_toolPalette = new ToolPalette(this);
    m_toolPalette->setFixedWidth(120);
    mainLayout->addWidget(m_toolPalette);

    // 2. Center: Scene & View
    m_scene = new FloorPlanScene(this);
    m_view = new FloorPlanView(m_scene, this);
    mainLayout->addWidget(m_view, 1);

    // 3. Right: Inspector + Session Controls
    QWidget *rightPanel = new QWidget(this);
    rightPanel->setFixedWidth(280);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    m_inspector = new InspectorPanel(this);
    rightLayout->addWidget(m_inspector, 1); // Stretch to fill top
    
    // Separator
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    rightLayout->addWidget(line);
    
    // Session Controls UI
    m_selectedSeatLabel = new QLabel("No seat selected", this);
    m_selectedSeatLabel->setStyleSheet("font-weight: bold; font-size: 14px; padding: 5px;");
    rightLayout->addWidget(m_selectedSeatLabel);
    
    m_startSessionBtn = new QPushButton("Start Session", this);
    m_startSessionBtn->setEnabled(false);
    m_startSessionBtn->setStyleSheet("background-color: #4CAF50; color: white; padding: 8px; border-radius: 4px;");
    rightLayout->addWidget(m_startSessionBtn);
    
    m_endSessionBtn = new QPushButton("End Session", this);
    m_endSessionBtn->setEnabled(false);
    m_endSessionBtn->setStyleSheet("background-color: #f44336; color: white; padding: 8px; border-radius: 4px;");
    rightLayout->addWidget(m_endSessionBtn);
    
    rightLayout->addStretch();
    mainLayout->addWidget(rightPanel);

    // Connections
    connect(m_scene, &QGraphicsScene::selectionChanged, this, &FloorPlanPage::handleSceneSelectionChanged);
    
    // FIXED: Use ObjectType instead of int
    connect(m_toolPalette, &ToolPalette::toolSelected, this, &FloorPlanPage::handleToolSelected);
    connect(m_toolPalette, &ToolPalette::selectModeRequested, this, &FloorPlanPage::handleSelectMode);
    
    connect(m_startSessionBtn, &QPushButton::clicked, this, &FloorPlanPage::handleStartSession);
    connect(m_endSessionBtn, &QPushButton::clicked, this, &FloorPlanPage::handleEndSession);
}

void FloorPlanPage::initializeSeatMap(const std::vector<Seat> &seats) {
    m_scene->clear(); 
    for (const auto &seat : seats) {
        SeatItem *item = new SeatItem(seat);
        m_scene->addItem(item);
    }
}

void FloorPlanPage::handleSceneSelectionChanged() {
    QList<QGraphicsItem*> selected = m_scene->selectedItems();
    if (selected.isEmpty()) {
        m_inspector->clearInspector();
        m_selectedSeatLabel->setText("No seat selected");
        m_startSessionBtn->setEnabled(false);
        m_endSessionBtn->setEnabled(false);
        m_currentSeatId = -1;
        return;
    }

    FloorPlanItem *item = dynamic_cast<FloorPlanItem*>(selected.first());
    m_inspector->setItem(item);

    if (item && item->objectType() == ObjectType::SEAT) {
        SeatItem *seatItem = qobject_cast<SeatItem*>(item);
        if (seatItem) {
            Seat seat = seatItem->seatData();
            m_currentSeatId = seat.id;
            m_selectedSeatLabel->setText(QString("Seat: %1 (%2)")
                .arg(seat.code)
                .arg(SeatStatusHelper::toString(seat.status)));
            
            // Enable/Disable buttons based on seat status
            if (seat.status == SeatStatus::Available) {
                m_startSessionBtn->setEnabled(true);
                m_endSessionBtn->setEnabled(false);
            } else if (seat.status == SeatStatus::Occupied) {
                m_startSessionBtn->setEnabled(false);
                m_endSessionBtn->setEnabled(true);
            } else {
                m_startSessionBtn->setEnabled(false);
                m_endSessionBtn->setEnabled(false);
            }
            
            // Emit signal for MainWindow
            emit seatSelected(seat);
        }
    } else {
        m_selectedSeatLabel->setText("Environment Object");
        m_startSessionBtn->setEnabled(false);
        m_endSessionBtn->setEnabled(false);
        m_currentSeatId = -1;
    }
}

void FloorPlanPage::handleToolSelected(ObjectType type, ShapeType shape) {
    m_view->setCurrentTool(type, shape);
}

void FloorPlanPage::handleSelectMode() {
    m_view->setSelectMode();
}

void FloorPlanPage::handleStartSession() {
    if (m_currentSeatId != -1) {
        emit startSessionRequested(m_currentSeatId);
    }
}

void FloorPlanPage::handleEndSession() {
    if (m_currentSeatId != -1) {
        emit endSessionRequested(m_currentSeatId);
    }
}