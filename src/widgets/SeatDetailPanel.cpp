#include "widgets/SeatDetailPanel.h"
#include "models/Seat.h"
#include "utils/Logger.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTimer>

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

    // Time display
    m_elapsedTimeLabel = new QLabel("Elapsed: --", this);
    m_elapsedTimeLabel->setStyleSheet("color: blue; font-weight: bold;");
    layout->addWidget(m_elapsedTimeLabel);

    m_remainingTimeLabel = new QLabel("Remaining: --", this);
    m_remainingTimeLabel->setStyleSheet("color: green; font-weight: bold;");
    layout->addWidget(m_remainingTimeLabel);

    // Extend buttons
    QHBoxLayout *extendLayout = new QHBoxLayout();
    m_extendBtn = new QPushButton("Add 30min", this);
    m_extendBtn->setEnabled(false);
    extendLayout->addWidget(m_extendBtn);
    
    QPushButton *add1HourBtn = new QPushButton("Add 1hr", this);
    add1HourBtn->setEnabled(false);
    extendLayout->addWidget(add1HourBtn);
    layout->addLayout(extendLayout);

    layout->addSpacing(10);

    // Seat info
    layout->addWidget(new QLabel("Code:", this));
    m_codeLabel = new QLabel("-", this);
    layout->addWidget(m_codeLabel);

    layout->addWidget(new QLabel("Status:", this));
    m_statusLabel = new QLabel("-", this);
    layout->addWidget(m_statusLabel);

    layout->addWidget(new QLabel("Area:", this));
    m_areaLabel = new QLabel("-", this);
    layout->addWidget(m_areaLabel);

    layout->addWidget(new QLabel("Capacity:", this));
    m_capacityLabel = new QLabel("-", this);
    layout->addWidget(m_capacityLabel);

    layout->addWidget(new QLabel("Note:", this));
    m_noteLabel = new QLabel("Please select a seat from the map.", this);
    m_noteLabel->setWordWrap(true);
    layout->addWidget(m_noteLabel);

    layout->addStretch();

    // Session buttons
    QHBoxLayout *sessionLayout = new QHBoxLayout();
    m_startBtn = new QPushButton("Start Session", this);
    m_startBtn->setEnabled(false);
    m_startBtn->setMinimumHeight(40);
    sessionLayout->addWidget(m_startBtn);

    m_endBtn = new QPushButton("End Session", this);
    m_endBtn->setEnabled(false);
    m_endBtn->setMinimumHeight(40);
    sessionLayout->addWidget(m_endBtn);
    layout->addLayout(sessionLayout);

    // Timer for elapsed time
    m_sessionTimer = new QTimer(this);
    connect(m_sessionTimer, &QTimer::timeout, this, [this]() {
        // In real app, fetch from service
        static int seconds = 0;
        seconds++;
        int mins = seconds / 60;
        int secs = seconds % 60;
        m_elapsedTimeLabel->setText(QString("Elapsed: %1m %2s").arg(mins).arg(secs, 2, 10, QChar('0')));
    });

    // Button connections
    connect(m_startBtn, &QPushButton::clicked, this, [this]() {
        if (m_currentSeatId != -1) {
            Logger::info("Start session requested for seat " + QString::number(m_currentSeatId));
            emit startSessionRequested(m_currentSeatId);
        }
    });

    connect(m_endBtn, &QPushButton::clicked, this, [this]() {
        if (m_currentSeatId != -1) {
            Logger::info("End session requested for seat " + QString::number(m_currentSeatId));
            emit endSessionRequested(m_currentSeatId);
        }
    });

    connect(m_extendBtn, &QPushButton::clicked, this, [this]() {
        if (m_currentSessionId != -1) {
            emit extendSessionRequested(m_currentSessionId, 30);
        }
    });

    connect(add1HourBtn, &QPushButton::clicked, this, [this]() {
        if (m_currentSessionId != -1) {
            emit extendSessionRequested(m_currentSessionId, 60);
        }
    });

}

void SeatDetailPanel::updateSeatInfo(const Seat &seat) {
    Logger::info("Updating seat info: " + seat.code);
    
    m_currentSeatId = seat.id;
    m_codeLabel->setText(seat.code);
    m_statusLabel->setText(SeatStatusHelper::toString(seat.status));
    m_areaLabel->setText(seat.area);
    m_capacityLabel->setText(QString::number(seat.capacity));
    m_noteLabel->setText(seat.note.isEmpty() ? "No notes" : seat.note);
    
    // Enable/disable buttons based on status
    if (seat.status == SeatStatus::Available) {
        m_startBtn->setEnabled(true);
        m_startBtn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;");
        m_endBtn->setEnabled(false);
        m_endBtn->setStyleSheet("");
        m_sessionTimer->stop();
        m_elapsedTimeLabel->setText("Elapsed: --");
        m_remainingTimeLabel->setText("Remaining: --");
    } else if (seat.status == SeatStatus::Occupied) {
        m_startBtn->setEnabled(false);
        m_startBtn->setStyleSheet("");
        m_endBtn->setEnabled(true);
        m_endBtn->setStyleSheet("background-color: #f44336; color: white; font-weight: bold;");
        m_sessionTimer->start(1000);
    } else {
        m_startBtn->setEnabled(false);
        m_endBtn->setEnabled(false);
        m_sessionTimer->stop();
    }
    
    updateButtons(seat);
}

void SeatDetailPanel::updateButtons(const Seat &seat) {
    if (seat.status == SeatStatus::Available) {
        m_startBtn->setEnabled(true);
        m_startBtn->setStyleSheet("background-color: #4CAF50; color: white;");
        m_endBtn->setEnabled(false);
        m_endBtn->setStyleSheet("");
    } else if (seat.status == SeatStatus::Occupied) {
        m_startBtn->setEnabled(false);
        m_startBtn->setStyleSheet("");
        m_endBtn->setEnabled(true);
        m_endBtn->setStyleSheet("background-color: #f44336; color: white;");
    } else {
        m_startBtn->setEnabled(false);
        m_endBtn->setEnabled(false);
    }
}

// ... modify clearSelection ...
void SeatDetailPanel::clearSelection() {
    m_currentSeatId = -1;
    m_currentSessionId = -1;
    m_codeLabel->setText("-");
    m_statusLabel->setText("-");
    m_areaLabel->setText("-");
    m_capacityLabel->setText("-");
    m_noteLabel->setText("Please select a seat from the map.");
    m_startBtn->setEnabled(false);
    m_endBtn->setEnabled(false);
    m_extendBtn->setEnabled(false);
    m_sessionTimer->stop();
    m_elapsedTimeLabel->setText("Elapsed: --");
    m_remainingTimeLabel->setText("Remaining: --");
}

void SeatDetailPanel::updateSessionTimeDisplay() {
    if (m_currentSessionId == -1) return;
    
    // Simplified: just show elapsed time
    // In real app, fetch from SeatSessionService
    static int elapsedSeconds = 0;
    elapsedSeconds++;
    
    int minutes = elapsedSeconds / 60;
    int seconds = elapsedSeconds % 60;
    
    m_elapsedTimeLabel->setText(QString("Elapsed: %1m %2s").arg(minutes).arg(seconds, 2, 10, QChar('0')));
}