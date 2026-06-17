#include "widgets/SeatMapView.h"
#include "widgets/SeatGraphicsItem.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>

SeatMapView::SeatMapView(QWidget *parent)
    : QWidget(parent) {
    m_scene = new QGraphicsScene(this);
    // Set a fixed scene rect for our demo map
    m_scene->setSceneRect(0, 0, 800, 600);

    m_view = new QGraphicsView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setDragMode(QGraphicsView::RubberBandDrag); // Allow rubber band selection

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_view);

    // Connect scene's selection changed signal to our custom signal
    connect(m_scene, &QGraphicsScene::selectionChanged, this, [this]() {
        QList<QGraphicsItem*> selected = m_scene->selectedItems();
        if (!selected.isEmpty()) {
            // We assume only one item is selected at a time for this demo
            SeatGraphicsItem *item = dynamic_cast<SeatGraphicsItem*>(selected.first());
            if (item) {
                emit seatSelected(item->seatData());
            }
        }
    });
}

SeatMapView::~SeatMapView() {
    // QGraphicsScene will automatically delete its items when destroyed.
}

void SeatMapView::loadSeats(const std::vector<Seat> &seats) {
    // Clear existing items
    m_scene->clear();
    m_items.clear();

    for (const auto &seat : seats) {
        SeatGraphicsItem *item = new SeatGraphicsItem(seat);
        m_scene->addItem(item);
        m_items.push_back(item);
    }
}