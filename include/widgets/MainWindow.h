#pragma once

#include "services/SeatService.h"
#include <QMainWindow>
#include <models/Seat.h>

class SeatMapView;
class SeatDetailPanel;
class SeatService;
class SeatSessionService;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initializeSeatMap(const std::vector<Seat> &seats);
    void setServices(SeatService *seatService, SeatSessionService *sessionService);

private slots:
    void handleStartSession(int seatId);
    void handleEndSession(int seatId);
    void refreshSeatMap();

private:
    void setupUI();
    
    SeatMapView *m_seatMapView;
    SeatDetailPanel *m_seatDetailPanel;
    SeatService *m_seatService = nullptr;
    SeatSessionService *m_sessionService = nullptr;
};