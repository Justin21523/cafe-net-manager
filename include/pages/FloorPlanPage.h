#pragma once
#include <QWidget>
#include <vector>
#include "models/Seat.h"
#include "graphics/FloorPlanItem.h"

class FloorPlanScene; class FloorPlanView; class ToolPalette; class InspectorPanel;
class QStackedWidget; class QPushButton; class QLabel;
class SeatSessionService; class OrderService; class SeatService;

class FloorPlanPage : public QWidget {
    Q_OBJECT
public:
    explicit FloorPlanPage(SeatService *seatSvc, SeatSessionService *sessionSvc, 
                           OrderService *orderSvc, QWidget *parent = nullptr);
    void initializeSeatMap(const std::vector<Seat> &seats);
    void refreshSeatMap();

signals:
    void seatSelected(const Seat &seat);
    void startSessionRequested(int seatId);
    void endSessionRequested(int seatId);
    void extendSessionRequested(int sessionId, int minutes);
    void seatPositionChanged(int seatId, int x, int y);
    void addOrderRequested(int seatId, const QString &seatCode);
    void checkoutRequested(int seatId);
    void sessionChanged();

private slots:
    void handleSceneSelectionChanged();
    void handleToolSelected(ObjectType type, ShapeType shape);
    void handleSelectMode();
    void handleEditModeToggled(bool isEditMode);
    void handleDeleteRequested();
    void handleSeatClickedInPreview(int seatId);
    void handleStartSession();
    void handleEndSession();

private:
    void setupUI();
    Seat findSeatById(int seatId) const;

    FloorPlanScene *m_scene;
    FloorPlanView *m_view;
    ToolPalette *m_toolPalette;
    InspectorPanel *m_inspector;
    QStackedWidget *m_rightStack;
    QWidget *m_operationPanel;

    QLabel *m_selectedSeatLabel;
    QPushButton *m_startSessionBtn;
    QPushButton *m_endSessionBtn;
    QPushButton *m_addOrderBtn;
    int m_currentSeatId = -1;
    bool m_isEditMode = false;

    SeatService *m_seatService;
    SeatSessionService *m_sessionService;
    OrderService *m_orderService;
    std::vector<Seat> m_seatsCache;
};
