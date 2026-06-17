#pragma once

#include <QWidget>
#include "models/Seat.h"

class QLabel;

class SeatDetailPanel : public QWidget {
    Q_OBJECT

public:
    explicit SeatDetailPanel(QWidget *parent = nullptr);

public slots:
    void updateSeatInfo(const Seat &seat);
    void clearSelection();

private:
    void setupUI();

    QLabel *m_codeLabel;
    QLabel *m_statusLabel;
    QLabel *m_areaLabel;
    QLabel *m_capacityLabel;
    QLabel *m_noteLabel;
};