#include "widgets/SeatDetailPanel.h"
#include "models/Seat.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QHBoxLayout>

SeatDetailPanel::SeatDetailPanel(QWidget *parent)
    : QWidget(parent) {
    setupUI();
    clearSelection();
}

void SeatDetailPanel::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QLabel *titleLabel = new QLabel("Seat Details", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    layout->addWidget(new QFrame()); // Separator line (simplified)

    m_codeLabel = new QLabel(this);
    m_statusLabel = new QLabel(this);
    m_areaLabel = new QLabel(this);
    m_capacityLabel = new QLabel(this);
    m_noteLabel = new QLabel(this);

    layout->addWidget(new QLabel("Code:", this));
    layout->addWidget(m_codeLabel);
    layout->addWidget(new QLabel("Status:", this));
    layout->addWidget(m_statusLabel);
    layout->addWidget(new QLabel("Area:", this));
    layout->addWidget(m_areaLabel);
    layout->addWidget(new QLabel("Capacity:", this));
    layout->addWidget(m_capacityLabel);
    layout->addWidget(new QLabel("Note:", this));
    layout->addWidget(m_noteLabel);

    layout->addStretch(); // Push content to the top
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_startBtn = new QPushButton("Start Session", this);
    m_endBtn = new QPushButton("End Session", this);
    btnLayout->addWidget(m_startBtn);
    btnLayout->addWidget(m_endBtn);
    layout->addLayout(btnLayout);

    connect(m_startBtn, &QPushButton::clicked, this, [this]() {
        if (m_currentSeatId != -1) emit startSessionRequested(m_currentSeatId);
    });
    connect(m_endBtn, &QPushButton::clicked, this, [this]() {
        if (m_currentSeatId != -1) emit endSessionRequested(m_currentSeatId);
    });

}

void SeatDetailPanel::updateSeatInfo(const Seat &seat) {
    m_currentSeatId = seat.id;
    m_codeLabel->setText(seat.code);
    m_statusLabel->setText(SeatStatusHelper::toString(seat.status));
    m_areaLabel->setText(seat.area);
    m_capacityLabel->setText(QString::number(seat.capacity));
    m_noteLabel->setText(seat.note.isEmpty() ? "No notes" : seat.note);
    updateButtons(seat);
}

void SeatDetailPanel::updateButtons(const Seat &seat) {
    if (seat.status == SeatStatus::Available) {
        m_startBtn->setEnabled(true);
        m_endBtn->setEnabled(false);
    } else if (seat.status == SeatStatus::Occupied) {
        m_startBtn->setEnabled(false);
        m_endBtn->setEnabled(true);
    } else {
        m_startBtn->setEnabled(false);
        m_endBtn->setEnabled(false);
    }
}

// ... modify clearSelection ...
void SeatDetailPanel::clearSelection() {
    m_currentSeatId = -1;
    m_codeLabel->setText("-");
    m_statusLabel->setText("-");
    m_areaLabel->setText("-");
    m_capacityLabel->setText("-");
    m_noteLabel->setText("Please select a seat from the map.");
    m_startBtn->setEnabled(false);
    m_endBtn->setEnabled(false);
}