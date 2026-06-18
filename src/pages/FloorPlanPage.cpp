#include "pages/FloorPlanPage.h"
#include "graphics/FloorPlanScene.h"
#include "widgets/FloorPlanView.h"
#include "widgets/ToolPalette.h"
#include "widgets/InspectorPanel.h"
#include "widgets/SeatOperationPanel.h"
#include "graphics/SeatItem.h"
#include "graphics/FloorPlanItem.h"

#include <QHBoxLayout>
#include <QStackedWidget>

FloorPlanPage::FloorPlanPage(SeatSessionService *sessionSvc, OrderService *orderSvc, QWidget *parent)
    : QWidget(parent) {
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

    // 3. Right: Inspector in edit mode, seat operations in preview mode
    m_rightStack = new QStackedWidget(this);
    m_rightStack->setFixedWidth(320);

    m_inspector = new InspectorPanel(this);
    m_operationPanel = new SeatOperationPanel(sessionSvc, orderSvc, this);

    m_rightStack->addWidget(m_operationPanel); // Preview mode
    m_rightStack->addWidget(m_inspector);      // Edit mode
    mainLayout->addWidget(m_rightStack);

    // Connections
    connect(m_scene, &QGraphicsScene::selectionChanged, this, &FloorPlanPage::handleSceneSelectionChanged);
    
    // FIXED: Use ObjectType instead of int
    connect(m_toolPalette, &ToolPalette::toolSelected, this, &FloorPlanPage::handleToolSelected);
    connect(m_toolPalette, &ToolPalette::selectModeRequested, this, &FloorPlanPage::handleSelectMode);
    connect(m_toolPalette, &ToolPalette::editModeToggled, this, &FloorPlanPage::handleEditModeToggled);
    
    connect(m_operationPanel, &SeatOperationPanel::startSessionRequested,
            this, &FloorPlanPage::handleOperationStartSession);
    connect(m_operationPanel, &SeatOperationPanel::endSessionRequested,
            this, &FloorPlanPage::handleOperationEndSession);
    connect(m_operationPanel, &SeatOperationPanel::checkoutRequested,
            this, &FloorPlanPage::checkoutRequested);
    connect(m_operationPanel, &SeatOperationPanel::addOrderRequested,
            this, [this](int seatId) {
                QList<QGraphicsItem*> selected = m_scene->selectedItems();
                if (selected.isEmpty()) return;

                SeatItem *seatItem = qobject_cast<SeatItem*>(dynamic_cast<FloorPlanItem*>(selected.first()));
                if (seatItem) {
                    emit addOrderRequested(seatId, seatItem->seatData().code);
                }
            });
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
        m_operationPanel->clearData();
        return;
    }

    FloorPlanItem *item = dynamic_cast<FloorPlanItem*>(selected.first());
    m_inspector->setItem(item);

    if (item && item->objectType() == ObjectType::SEAT) {
        SeatItem *seatItem = qobject_cast<SeatItem*>(item);
        if (seatItem) {
            Seat seat = seatItem->seatData();
            m_operationPanel->loadSeatData(seat);
            // Emit signal for MainWindow
            emit seatSelected(seat);
        }
    } else {
        m_operationPanel->clearData();
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
    m_rightStack->setCurrentWidget(isEditMode ? static_cast<QWidget*>(m_inspector)
                                              : static_cast<QWidget*>(m_operationPanel));
}

void FloorPlanPage::handleOperationStartSession(int seatId) {
    emit startSessionRequested(seatId);
    emit sessionChanged();
}

void FloorPlanPage::handleOperationEndSession(int seatId) {
    emit endSessionRequested(seatId);
    emit sessionChanged();
}
