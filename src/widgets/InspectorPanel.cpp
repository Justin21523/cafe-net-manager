#include "widgets/InspectorPanel.h"
#include "graphics/FloorPlanItem.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

InspectorPanel::InspectorPanel(QWidget *parent) : QWidget(parent) {
    setStyleSheet("background-color: #1e1e1e; color: #ffffff;");
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_titleLabel = new QLabel("Inspector", this);
    QFont font = m_titleLabel->font();
    font.setPointSize(14);
    font.setBold(true);
    m_titleLabel->setFont(font);
    m_titleLabel->setStyleSheet("color: #ffffff; padding: 10px 0;");
    layout->addWidget(m_titleLabel);
    
    m_formLayout = new QFormLayout();
    layout->addLayout(m_formLayout);
    layout->addStretch();
    clearInspector();
}

void InspectorPanel::setItem(FloorPlanItem *item) {
    if (m_currentItem) {
        disconnect(m_currentItem, &FloorPlanItem::geometryChanged, this, &InspectorPanel::handleGeometryChanged);
    }

    while (m_formLayout->count() > 0) {
        QLayoutItem *child = m_formLayout->takeAt(0);
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }

    m_currentItem = item;
    if (!item) {
        clearInspector();
        return;
    }

    m_titleLabel->setText(item->displayName());
    buildUI(item);

    connect(item, &FloorPlanItem::geometryChanged, this, &InspectorPanel::handleGeometryChanged);
    updateUIFromItem();
}

void InspectorPanel::buildUI(FloorPlanItem *item) {
    m_xSpin = new QSpinBox(this);
    m_xSpin->setRange(-2000, 4000);
    m_ySpin = new QSpinBox(this);
    m_ySpin->setRange(-2000, 4000);
    m_wSpin = new QSpinBox(this);
    m_wSpin->setRange(20, 2000);
    m_hSpin = new QSpinBox(this);
    m_hSpin->setRange(20, 2000);
    m_shapeCombo = new QComboBox(this);
    m_shapeCombo->addItem("Rectangle", (int)ShapeType::Rectangle);
    m_shapeCombo->addItem("Circle", (int)ShapeType::Circle);
    m_shapeCombo->addItem("Triangle", (int)ShapeType::Triangle);

    // Apply dark theme styles
    QString spinStyle = R"(
        QSpinBox {
            background-color: #333333;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 3px;
            padding: 4px;
            font-size: 12px;
        }
        QSpinBox:focus {
            border: 1px solid #4CAF50;
        }
    )";
    
    QString comboStyle = R"(
        QComboBox {
            background-color: #333333;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 3px;
            padding: 4px;
            font-size: 12px;
        }
        QComboBox:focus {
            border: 1px solid #4CAF50;
        }
        QComboBox::drop-down {
            background-color: #444444;
        }
    )";

    m_xSpin->setStyleSheet(spinStyle);
    m_ySpin->setStyleSheet(spinStyle);
    m_wSpin->setStyleSheet(spinStyle);
    m_hSpin->setStyleSheet(spinStyle);
    m_shapeCombo->setStyleSheet(comboStyle);

    m_xSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_ySpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_wSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_hSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_shapeCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Style the form labels
    m_formLayout->setLabelAlignment(Qt::AlignRight);
    m_formLayout->setFormAlignment(Qt::AlignLeft);
    
    // Apply style to labels
    for (int i = 0; i < m_formLayout->rowCount(); ++i) {
        QLayoutItem *labelItem = m_formLayout->itemAt(i, QFormLayout::LabelRole);
        if (labelItem && labelItem->widget()) {
            labelItem->widget()->setStyleSheet("color: #cccccc; font-size: 12px;");
        }
    }

    m_formLayout->addRow("X:", m_xSpin);
    m_formLayout->addRow("Y:", m_ySpin);
    m_formLayout->addRow("Width:", m_wSpin);
    m_formLayout->addRow("Height:", m_hSpin);
    m_formLayout->addRow("Shape:", m_shapeCombo);

    connect(m_xSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &InspectorPanel::handleSpinBoxChanged);
    connect(m_ySpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &InspectorPanel::handleSpinBoxChanged);
    connect(m_wSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &InspectorPanel::handleSpinBoxChanged);
    connect(m_hSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &InspectorPanel::handleSpinBoxChanged);
    connect(m_shapeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &InspectorPanel::handleSpinBoxChanged);
}

void InspectorPanel::updateUIFromItem() {
    if (!m_currentItem || m_isUpdating) return;
    m_isUpdating = true;

    m_xSpin->blockSignals(true);
    m_ySpin->blockSignals(true);
    m_wSpin->blockSignals(true);
    m_hSpin->blockSignals(true);
    m_shapeCombo->blockSignals(true);

    m_xSpin->setValue(static_cast<int>(m_currentItem->pos().x()));
    m_ySpin->setValue(static_cast<int>(m_currentItem->pos().y()));
    m_wSpin->setValue(static_cast<int>(m_currentItem->size().width()));
    m_hSpin->setValue(static_cast<int>(m_currentItem->size().height()));
    m_shapeCombo->setCurrentIndex(m_shapeCombo->findData(static_cast<int>(m_currentItem->shapeType())));

    m_xSpin->blockSignals(false);
    m_ySpin->blockSignals(false);
    m_wSpin->blockSignals(false);
    m_hSpin->blockSignals(false);
    m_shapeCombo->blockSignals(false);

    m_isUpdating = false;
}

void InspectorPanel::handleGeometryChanged() {
    updateUIFromItem();
}

void InspectorPanel::handleSpinBoxChanged() {
    if (!m_currentItem || m_isUpdating) return;
    m_currentItem->setPos(m_xSpin->value(), m_ySpin->value());
    m_currentItem->setSize(QSizeF(m_wSpin->value(), m_hSpin->value()));
    m_currentItem->setShapeType(static_cast<ShapeType>(m_shapeCombo->currentData().toInt()));
}

void InspectorPanel::clearInspector() {
    m_titleLabel->setText("Inspector (No Selection)");
    m_currentItem = nullptr;
}
