#pragma once
#include <QDialog>
#include "models/MenuItem.h"

class QComboBox;
class QLineEdit;
class QLabel;

class ModifierDialog : public QDialog {
    Q_OBJECT
public:
    explicit ModifierDialog(const MenuItem &item, QWidget *parent = nullptr);
    
    QString getModifiers() const;

private:
    void setupUI();
    MenuItem m_item;
    QComboBox *m_tempCombo;
    QLineEdit *m_noteEdit;
    QLabel *m_itemNameLabel;
};