#pragma once

#include <QWidget>
#include <vector>
#include "models/Seat.h"

class QGraphicsView;
class QGraphicsScene;
class SeatGraphicsItem;

class SeatMapView : public QWidget {
    Q_OBJECT

public:
    explicit SeatMapView(QWidget *parent = nullptr);
    ~SeatMapView();

    void loadSeats(const std::vector<Seat> &seats);

signals:
    void seatSelected(const Seat &seat);

private:
    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    std::vector<SeatGraphicsItem*> m_items; // Keep track to prevent memory leaks if needed
};