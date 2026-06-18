#include "pages/FloorPlanPage.h"
#include "graphics/FloorPlanScene.h"
#include "widgets/FloorPlanView.h"
#include "widgets/ToolPalette.h"
#include "widgets/InspectorPanel.h"
#include "widgets/SeatInfoDialog.h"
#include "graphics/SeatItem.h"
#include "services/SeatService.h"
#include "services/SeatSessionService.h"
#include "services/OrderService.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QStackedWidget>

FloorPlanPage::FloorPlanPage(SeatService *seatSvc, SeatSessionService *sessionSvc,
                             OrderService *orderSvc, QWidget *parent)
    : QWidget(parent), m_seatService(seatSvc), m_sessionService(sessionSvc), m_orderService(orderSvc) {
    setupUI();
}

void FloorPlanPage::setupUI() {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 1. Left: Tool Palette
    m_toolPalette = new ToolPalette(this);
    mainLayout->addWidget(m_toolPalette);

    // 2. Center: Scene & View
    m_scene = new FloorPlanScene(this);
    m_view = new FloorPlanView(m_scene, this);
    mainLayout->addWidget(m_view, 1);

    // 3. Right: Stacked (Inspector / Operation)
    m_rightStack = new QStackedWidget(this);
    m_rightStack->setFixedWidth(300);
    m_rightStack->setStyleSheet("background-color: #1e1e1e; border-left: 1px solid #333;");

    // Page 0: Inspector
    m_inspector = new InspectorPanel(this);
    m_rightStack->addWidget(m_inspector);

    // Page 1: Operation Panel
    m_operationPanel = new QWidget(this);
    m_operationPanel->setStyleSheet("background-color: #1e1e1e;");
    QVBoxLayout *opLayout = new QVBoxLayout(m_operationPanel);
    opLayout->setContentsMargins(15, 15, 15, 15);
    opLayout->setSpacing(12);

    m_selectedSeatLabel = new QLabel("No seat selected", this);
    m_selectedSeatLabel->setStyleSheet("font-weight: bold; font-size: 16px; padding: 8px; color: #ffffff; background-color: #2d2d2d; border-radius: 4px;");
    m_selectedSeatLabel->setWordWrap(true);
    opLayout->addWidget(m_selectedSeatLabel);

    m_startSessionBtn = new QPushButton("▶️ Start Session", this);
    m_startSessionBtn->setMinimumHeight(42);
    m_startSessionBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;
            color: #ffffff;
            font-weight: bold;
            border-radius: 4px;
            font-size: 14px;
            border: none;
        }
        QPushButton:hover {
            background-color: #388E3C;
        }
        QPushButton:disabled {
            background-color: #555555;
            color: #888888;
        }
    )");
    opLayout->addWidget(m_startSessionBtn);

    m_endSessionBtn = new QPushButton("⏹️ End Session", this);
    m_endSessionBtn->setMinimumHeight(42);
    m_endSessionBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #f44336;
            color: #ffffff;
            font-weight: bold;
            border-radius: 4px;
            font-size: 14px;
            border: none;
        }
        QPushButton:hover {
            background-color: #d32f2f;
        }
        QPushButton:disabled {
            background-color: #555555;
            color: #888888;
        }
    )");
    opLayout->addWidget(m_endSessionBtn);

    m_addOrderBtn = new QPushButton("🛒 Add Order (POS)", this);
    m_addOrderBtn->setMinimumHeight(42);
    m_addOrderBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #2196F3;
            color: #ffffff;
            font-weight: bold;
            border-radius: 4px;
            font-size: 14px;
            border: none;
        }
        QPushButton:hover {
            background-color: #1976D2;
        }
        QPushButton:disabled {
            background-color: #555555;
            color: #888888;
        }
    )");
    opLayout->addWidget(m_addOrderBtn);

    opLayout->addStretch();
    m_rightStack->addWidget(m_operationPanel);

    mainLayout->addWidget(m_rightStack);
    m_rightStack->setCurrentIndex(1);

    // Connections
    connect(m_scene, &QGraphicsScene::selectionChanged, this, &FloorPlanPage::handleSceneSelectionChanged);
    connect(m_toolPalette, &ToolPalette::toolSelected, this, &FloorPlanPage::handleToolSelected);
    connect(m_toolPalette, &ToolPalette::selectModeRequested, this, &FloorPlanPage::handleSelectMode);
    connect(m_toolPalette, &ToolPalette::editModeToggled, this, &FloorPlanPage::handleEditModeToggled);
    connect(m_toolPalette, &ToolPalette::deleteRequested, this, &FloorPlanPage::handleDeleteRequested);
    connect(m_view, &FloorPlanView::seatClickedInPreview, this, &FloorPlanPage::handleSeatClickedInPreview);
    connect(m_startSessionBtn, &QPushButton::clicked, this, &FloorPlanPage::handleStartSession);
    connect(m_endSessionBtn, &QPushButton::clicked, this, &FloorPlanPage::handleEndSession);
    connect(m_addOrderBtn, &QPushButton::clicked, this, [this]() {
        if (m_currentSeatId != -1) {
            Seat s = findSeatById(m_currentSeatId);
            emit addOrderRequested(m_currentSeatId, s.code);
        }
    });
}

void FloorPlanPage::initializeSeatMap(const std::vector<Seat> &seats) {
    m_seatsCache = seats;
    m_scene->clear();
    for (const auto &seat : seats) {
        SeatItem *item = new SeatItem(seat);
        item->setEditMode(m_isEditMode);
        m_scene->addItem(item);
    }
}

void FloorPlanPage::refreshSeatMap() {
    if (m_seatService) {
        initializeSeatMap(m_seatService->loadAllSeats());
    }
}

Seat FloorPlanPage::findSeatById(int seatId) const {
    for (const auto &s : m_seatsCache) {
        if (s.id == seatId) return s;
    }
    return Seat();
}

void FloorPlanPage::handleSceneSelectionChanged() {
    QList<QGraphicsItem*> selected = m_scene->selectedItems();
    if (selected.isEmpty()) {
        if (m_isEditMode) m_inspector->clearInspector();
        m_selectedSeatLabel->setText("No seat selected");
        m_startSessionBtn->setEnabled(false);
        m_endSessionBtn->setEnabled(false);
        m_addOrderBtn->setEnabled(false);
        m_currentSeatId = -1;
        return;
    }

    FloorPlanItem *item = dynamic_cast<FloorPlanItem*>(selected.first());
    if (!item) return;

    if (m_isEditMode) {
        m_rightStack->setCurrentWidget(m_inspector);
        m_inspector->setItem(item);
    } else {
        m_rightStack->setCurrentWidget(m_operationPanel);
    }

    if (item->objectType() == ObjectType::SEAT) {
        SeatItem *seatItem = qobject_cast<SeatItem*>(item);
        if (seatItem) {
            Seat seat = seatItem->seatData();
            m_currentSeatId = seat.id;
            QString statusStr = SeatStatusHelper::toString(seat.status);
            m_selectedSeatLabel->setText(QString("🪑 %1 (%2)").arg(seat.code).arg(statusStr));
            m_startSessionBtn->setEnabled(seat.status == SeatStatus::Available);
            m_endSessionBtn->setEnabled(seat.status == SeatStatus::Occupied);
            m_addOrderBtn->setEnabled(seat.status == SeatStatus::Occupied);
            emit seatSelected(seat);
        }
    } else {
        m_selectedSeatLabel->setText("🔧 " + item->displayName());
        m_startSessionBtn->setEnabled(false);
        m_endSessionBtn->setEnabled(false);
        m_addOrderBtn->setEnabled(false);
    }
}

void FloorPlanPage::handleToolSelected(ObjectType type, ShapeType shape) {
    m_view->setCurrentTool(type, shape);
}

void FloorPlanPage::handleSelectMode() {
    m_view->setSelectMode();
}

void FloorPlanPage::handleEditModeToggled(bool isEditMode) {
    m_isEditMode = isEditMode;
    m_view->setEditMode(isEditMode);
    if (isEditMode) {
        m_rightStack->setCurrentWidget(m_inspector);
    } else {
        m_rightStack->setCurrentWidget(m_operationPanel);
    }
}

void FloorPlanPage::handleDeleteRequested() {
    if (!m_isEditMode) return;
    QList<QGraphicsItem*> selected = m_scene->selectedItems();
    for (QGraphicsItem* item : selected) {
        m_scene->removeItem(item);
        delete item;
    }
}

void FloorPlanPage::handleSeatClickedInPreview(int seatId) {
    Seat seat = findSeatById(seatId);
    if (seat.id > 0) {
        SeatInfoDialog dialog(seat, this);
        dialog.exec();
    }
}

void FloorPlanPage::handleStartSession() {
    if (m_currentSeatId != -1) {
        emit startSessionRequested(m_currentSeatId);
        emit sessionChanged();
        refreshSeatMap();
    }
}

void FloorPlanPage::handleEndSession() {
    if (m_currentSeatId != -1) {
        emit endSessionRequested(m_currentSeatId);
        emit sessionChanged();
        refreshSeatMap();
    }
}
