#pragma once
#include <QWidget>
#include <vector>
#include "models/Seat.h"

class SeatMapView;
class SeatDetailPanel;

class FloorPlanPage : public QWidget {
    Q_OBJECT
public:
    explicit FloorPlanPage(QWidget *parent = nullptr);
    void initializeSeatMap(const std::vector<Seat> &seats);

signals:
    void seatSelected(const Seat &seat);
    void startSessionRequested(int seatId);
    void endSessionRequested(int seatId);
    void extendSessionRequested(int sessionId, int minutes);
    void seatPositionChanged(int seatId, int x, int y);

private:
    SeatMapView* m_seatMapView;
    SeatDetailPanel* m_seatDetailPanel;
};