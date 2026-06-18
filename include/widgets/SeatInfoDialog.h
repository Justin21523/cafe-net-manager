#pragma once
#include <QDialog>
#include "models/Seat.h"
class QLabel;

class SeatInfoDialog : public QDialog {
    Q_OBJECT
public:
    explicit SeatInfoDialog(const Seat &seat, QWidget *parent = nullptr);
private:
    void setupUI();
    Seat m_seat;
};