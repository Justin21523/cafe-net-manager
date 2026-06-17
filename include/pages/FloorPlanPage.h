#pragma once

#include <QWidget>
#include <vector>
#include "models/Seat.h"
#include "graphics/FloorPlanItem.h" // For ObjectType

class FloorPlanScene;
class FloorPlanView;
class ToolPalette;
class InspectorPanel;
class SeatItem;
class QPushButton;
class QLabel;

class FloorPlanPage : public QWidget {
    Q_OBJECT
public:
    explicit FloorPlanPage(QWidget *parent = nullptr);
    void initializeSeatMap(const std::vector<Seat> &seats);

signals:
    // These signals are connected to MainWindow in V2.0
    void seatSelected(const Seat &seat);
    void startSessionRequested(int seatId);
    void endSessionRequested(int seatId);
    void extendSessionRequested(int sessionId, int minutes);
    void seatPositionChanged(int seatId, int x, int y);

private slots:
    void handleSceneSelectionChanged();
    void handleToolSelected(ObjectType type); // FIXED: Match ToolPalette's signal type
    void handleSelectMode();
    void handleStartSession();
    void handleEndSession();

private:
    FloorPlanScene *m_scene;
    FloorPlanView *m_view;
    ToolPalette *m_toolPalette;
    InspectorPanel *m_inspector;
    
    // Session Controls (Restored)
    QLabel *m_selectedSeatLabel;
    QPushButton *m_startSessionBtn;
    QPushButton *m_endSessionBtn;
    int m_currentSeatId = -1;
};