#pragma once
#include <QWidget>
#include <vector>
#include "models/Seat.h"

class FloorPlanScene;
class FloorPlanView;
class ToolPalette;
class InspectorPanel;
class SeatItem;

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

private slots:
    void handleSceneSelectionChanged();
    void handleToolSelected(ObjectType type, ShapeType shape);
    void handleSelectMode();

private:
    FloorPlanScene *m_scene;
    FloorPlanView *m_view;
    ToolPalette *m_toolPalette;
    InspectorPanel *m_inspector;
};