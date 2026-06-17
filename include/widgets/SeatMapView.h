#pragma once

#include <QWidget>
#include <QPushButton>
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
    void setEditMode(bool enabled);
    void saveLayout();

signals:
    void seatSelected(const Seat &seat);
    void seatPositionChanged(int seatId, int x, int y); // Add this
    
private:
    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    std::vector<SeatGraphicsItem*> m_items; // Keep track to prevent memory leaks if needed
    QPushButton *m_editToggleBtn;
    QPushButton *m_saveBtn;
    bool m_isEditMode = false;
};