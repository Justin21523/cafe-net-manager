#include "pages/FloorPlanPage.h"
#include "graphics/FloorPlanScene.h"
#include "widgets/FloorPlanView.h"
#include "widgets/ToolPalette.h"
#include "widgets/InspectorPanel.h"
#include "graphics/SeatItem.h"
#include "graphics/FloorPlanItem.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <QVBoxLayout>

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
    mainLayout->addWidget(m_view, 1); // Stretch

    // 3. Right: Inspector
    m_inspector = new InspectorPanel(this);
    m_inspector->setFixedWidth(250);
    mainLayout->addWidget(m_inspector);

    // Connections
    connect(m_scene, &QGraphicsScene::selectionChanged, this, &FloorPlanPage::handleSceneSelectionChanged);
    connect(m_toolPalette, &ToolPalette::toolSelected, this, [this](ObjectType type, ShapeType shape) {
        m_view->setCurrentTool(type, shape);
    });

    connect(m_toolPalette, &ToolPalette::editModeToggled, this, [this](bool isEditMode) {
        m_view->setEditMode(isEditMode);
        // In preview mode, clear selection to prevent handles showing
        if (!isEditMode) m_scene->clearSelection(); 
    });
}

void FloorPlanPage::initializeSeatMap(const std::vector<Seat> &seats) {
    m_scene->clear(); // Clear all items including walls
    for (const auto &seat : seats) {
        SeatItem *item = new SeatItem(seat);
        m_scene->addItem(item);
    }
}

void FloorPlanPage::handleSceneSelectionChanged() {
    QList<QGraphicsItem*> selected = m_scene->selectedItems();
    if (selected.isEmpty()) {
        m_inspector->clearInspector();
        return;
    }

    FloorPlanItem *item = dynamic_cast<FloorPlanItem*>(selected.first());
    m_inspector->setItem(item);

    // If it's a seat, emit the old signal for backward compatibility with MainWindow
    if (item && item->objectType() == ObjectType::SEAT) {
        SeatItem *seatItem = qobject_cast<SeatItem*>(item);
        if (seatItem) emit seatSelected(seatItem->seatData());
    }
}

void FloorPlanPage::handleToolSelected(int type) {
    m_view->setCurrentTool(static_cast<ObjectType>(type));
}

void FloorPlanPage::handleSelectMode() {
    m_view->setSelectMode();
}