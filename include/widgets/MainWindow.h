#pragma once

#include <QMainWindow>
#include <models/Seat.h>

class SeatMapView;
class SeatDetailPanel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initializeSeatMap(const std::vector<Seat> &seats);

private:
    void setupUI();
    
    SeatMapView *m_seatMapView;
    SeatDetailPanel *m_seatDetailPanel;
};