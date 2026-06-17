#pragma once

#include <QWidget>
#include <QPushButton>
#include "models/Seat.h"

class QLabel;
class QPushButton;
class QTimer;

class SeatDetailPanel : public QWidget {
    Q_OBJECT

public:
    explicit SeatDetailPanel(QWidget *parent = nullptr);

public slots:
    void updateSeatInfo(const Seat &seat);
    void clearSelection();

signals:
    void startSessionRequested(int seatId);
    void endSessionRequested(int seatId);
    void extendSessionRequested(int sessionId, int minutes);

private slots:
    void updateSessionTimeDisplay();  // Added declaration

private:
    void setupUI();
    void updateButtons(const Seat &seat);

    QLabel *m_codeLabel;
    QLabel *m_statusLabel;
    QLabel *m_areaLabel;
    QLabel *m_capacityLabel;
    QLabel *m_noteLabel;
    QPushButton *m_startBtn;
    QPushButton *m_endBtn;
    int m_currentSeatId = -1;
    QLabel *m_elapsedTimeLabel;
    QLabel *m_remainingTimeLabel;
    QPushButton *m_extendBtn;
    QTimer *m_sessionTimer;
    int m_currentSessionId = -1;
};