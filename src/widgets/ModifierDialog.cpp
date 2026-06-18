#include "widgets/ModifierDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>

ModifierDialog::ModifierDialog(const MenuItem &item, QWidget *parent)
    : QDialog(parent), m_item(item) {
    setupUI();
}

void ModifierDialog::setupUI() {
    setWindowTitle("Customize Item");
    setMinimumWidth(350);

    QVBoxLayout *layout = new QVBoxLayout(this);

    m_itemNameLabel = new QLabel(m_item.name, this);
    QFont font = m_itemNameLabel->font();
    font.setPointSize(14); font.setBold(true);
    m_itemNameLabel->setFont(font);
    layout->addWidget(m_itemNameLabel);

    // Temperature (Dummy modifier for demo)
    QHBoxLayout *tempLayout = new QHBoxLayout();
    tempLayout->addWidget(new QLabel("Temperature:", this));
    m_tempCombo = new QComboBox(this);
    m_tempCombo->addItems({"Hot", "Iced", "Warm"});
    tempLayout->addWidget(m_tempCombo);
    layout->addLayout(tempLayout);

    // Special Note
    layout->addWidget(new QLabel("Special Note / Allergies:", this));
    m_noteEdit = new QLineEdit(this);
    m_noteEdit->setPlaceholderText("e.g., No sugar, Oat milk...");
    layout->addWidget(m_noteEdit);

    // Buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}

QString ModifierDialog::getModifiers() const {
    QString mods = m_tempCombo->currentText();
    QString note = m_noteEdit->text().trimmed();
    if (!note.isEmpty()) {
        mods += " | " + note;
    }
    return mods;
}