#pragma once

#include <QWidget>
#include <vector>
#include "models/Seat.h"
#include "graphics/FloorPlanItem.h"

class FloorPlanScene;
class FloorPlanView;
class ToolPalette;
class InspectorPanel;
class SeatOperationPanel; // NEW
class QStackedWidget;     // NEW
class SeatItem;
class QPushButton;
class QLabel;
class SeatSessionService; // NEW
class OrderService;       // NEW

class FloorPlanPage : public QWidget {
    Q_OBJECT
public:
    // 修改建構子，注入需要的 Service
    explicit FloorPlanPage(SeatSessionService *sessionSvc, OrderService *orderSvc, QWidget *parent = nullptr);
    void initializeSeatMap(const std::vector<Seat> &seats);

signals:
    void seatSelected(const Seat &seat);
    void startSessionRequested(int seatId);
    void endSessionRequested(int seatId);
    void extendSessionRequested(int sessionId, int minutes);
    void seatPositionChanged(int seatId, int x, int y);
    void checkoutRequested(int seatId);      // NEW
    void addOrderRequested(int seatId, const QString &seatCode);
    void sessionChanged();

private slots:
    void handleSceneSelectionChanged();
    void handleToolSelected(ObjectType type, ShapeType shape);
    void handleSelectMode();
    void handleEditModeToggled(bool isEditMode);
    
    // Forward signals from SeatOperationPanel
    void handleOperationStartSession(int seatId);
    void handleOperationEndSession(int seatId);

private:
    FloorPlanScene *m_scene;
    FloorPlanView *m_view;
    ToolPalette *m_toolPalette;
    
    // Right side panels
    QStackedWidget *m_rightStack;
    InspectorPanel *m_inspector;
    SeatOperationPanel *m_operationPanel; // NEW
    
    bool m_isEditMode = false;
};
