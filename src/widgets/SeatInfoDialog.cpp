#include "widgets/SeatInfoDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QFont>

SeatInfoDialog::SeatInfoDialog(const Seat &seat, QWidget *parent)
    : QDialog(parent), m_seat(seat) {
    setupUI();
}

void SeatInfoDialog::setupUI() {
    setWindowTitle("Seat Info - " + m_seat.code);
    setMinimumWidth(350);
    setModal(true);
    setStyleSheet("background-color: white;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(20, 20, 20, 20);

    // Header
    QHBoxLayout *header = new QHBoxLayout();
    QLabel *codeLabel = new QLabel("🪑 " + m_seat.code, this);
    QFont titleFont = codeLabel->font();
    titleFont.setPointSize(20); titleFont.setBold(true);
    codeLabel->setFont(titleFont);
    header->addWidget(codeLabel);

    QString statusStr;
    QString statusColor;
    switch (m_seat.status) {
        case SeatStatus::Available: statusStr = "Available"; statusColor = "#4CAF50"; break;
        case SeatStatus::Occupied:  statusStr = "Occupied";  statusColor = "#f44336"; break;
        case SeatStatus::Reserved:  statusStr = "Reserved";  statusColor = "#2196F3"; break;
        case SeatStatus::Cleaning:  statusStr = "Cleaning";  statusColor = "#FF9800"; break;
        case SeatStatus::Disabled:  statusStr = "Disabled";  statusColor = "#9E9E9E"; break;
    }

    QLabel *badge = new QLabel(statusStr, this);
    badge->setStyleSheet(QString("background-color: %1; color: white; padding: 6px 12px; border-radius: 4px; font-weight: bold; font-size: 14px;").arg(statusColor));
    header->addStretch();
    header->addWidget(badge);
    layout->addLayout(header);

    // Separator
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #ddd;");
    layout->addWidget(line);

    // Info Grid
    auto addRow = [&](const QString &label, const QString &value) {
        QHBoxLayout *row = new QHBoxLayout();
        QLabel *lbl = new QLabel(label + ":", this);
        lbl->setStyleSheet("font-weight: bold; color: #555; min-width: 100px;");
        QLabel *val = new QLabel(value, this);
        val->setStyleSheet("color: #333;");
        row->addWidget(lbl);
        row->addWidget(val);
        row->addStretch();
        layout->addLayout(row);
    };

    addRow("Name", m_seat.name.isEmpty() ? "-" : m_seat.name);
    addRow("Area", m_seat.area.isEmpty() ? "-" : m_seat.area);
    addRow("Type", m_seat.type.isEmpty() ? "-" : m_seat.type);
    addRow("Capacity", QString::number(m_seat.capacity));
    addRow("Power Outlet", m_seat.hasPowerOutlet ? "✅ Yes" : "❌ No");
    addRow("Quiet Zone", m_seat.isQuietZone ? "✅ Yes" : "❌ No");
    addRow("Note", m_seat.note.isEmpty() ? "-" : m_seat.note);

    // If occupied, show customer section
    if (m_seat.status == SeatStatus::Occupied) {
        layout->addSpacing(10);
        QFrame *line2 = new QFrame(this);
        line2->setFrameShape(QFrame::HLine);
        line2->setStyleSheet("color: #ddd;");
        layout->addWidget(line2);

        QLabel *custTitle = new QLabel("👤 Current Customer", this);
        custTitle->setStyleSheet("font-weight: bold; font-size: 14px; color: #d32f2f;");
        layout->addWidget(custTitle);

        addRow("Status", "Currently seated (session active)");
        addRow("Action", "Switch to Edit Mode to manage session");
    }

    layout->addStretch();

    // Close button
    QPushButton *closeBtn = new QPushButton("Close", this);
    closeBtn->setMinimumHeight(40);
    closeBtn->setStyleSheet("background-color: #2196F3; color: white; font-size: 14px; font-weight: bold; border-radius: 4px;");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeBtn);
}