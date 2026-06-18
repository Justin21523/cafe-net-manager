#include "widgets/InspectorPanel.h"
#include "graphics/FloorPlanItem.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>

InspectorPanel::InspectorPanel(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_titleLabel = new QLabel("Inspector", this);
    QFont font = m_titleLabel->font(); font.setPointSize(14); font.setBold(true);
    m_titleLabel->setFont(font);
    layout->addWidget(m_titleLabel);

    m_formLayout = new QFormLayout();
    layout->addLayout(m_formLayout);
    layout->addStretch();
    clearInspector();
}

void InspectorPanel::setItem(FloorPlanItem *item) {
    if (m_currentItem) disconnect(m_currentItem, &FloorPlanItem::geometryChanged, this, &InspectorPanel::handleGeometryChanged);
    
    m_currentItem = item;
    QLayoutItem *child;
    while ((child = m_formLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }

    if (!item) { clearInspector(); return; }

    m_titleLabel->setText(item->displayName());
    buildUI(item);
    
    connect(item, &FloorPlanItem::geometryChanged, this, &InspectorPanel::handleGeometryChanged);
    updateUIFromItem();
}

void InspectorPanel::buildUI(FloorPlanItem *item) {
    m_xSpin = new QSpinBox(this); m_xSpin->setRange(-1000, 2000);
    m_ySpin = new QSpinBox(this); m_ySpin->setRange(-1000, 2000);
    m_wSpin = new QSpinBox(this); m_wSpin->setRange(20, 1000);
    m_hSpin = new QSpinBox(this); m_hSpin->setRange(20, 1000);
    
    m_shapeCombo = new QComboBox(this);
    m_shapeCombo->addItem("Rectangle", (int)ShapeType::Rectangle);
    m_shapeCombo->addItem("Circle", (int)ShapeType::Circle);
    m_shapeCombo->addItem("Triangle", (int)ShapeType::Triangle);
    
    // FIX 4: Ensure widgets expand to fill available width
    m_xSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_ySpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_wSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_hSpin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_shapeCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

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
    m_xSpin->blockSignals(true); m_ySpin->blockSignals(true);
    m_wSpin->blockSignals(true); m_hSpin->blockSignals(true);
    m_shapeCombo->blockSignals(true);

    m_xSpin->setValue(m_currentItem->pos().x());
    m_ySpin->setValue(m_currentItem->pos().y());
    m_wSpin->setValue(m_currentItem->size().width());
    m_hSpin->setValue(m_currentItem->size().height());
    m_shapeCombo->setCurrentIndex(m_shapeCombo->findData((int)m_currentItem->shapeType()));

    m_xSpin->blockSignals(false); m_ySpin->blockSignals(false);
    m_wSpin->blockSignals(false); m_hSpin->blockSignals(false);
    m_shapeCombo->blockSignals(false);
    m_isUpdating = false;
}

void InspectorPanel::handleGeometryChanged() { updateUIFromItem(); }

void InspectorPanel::handleSpinBoxChanged() {
    if (!m_currentItem || m_isUpdating) return;
    m_currentItem->setPos(m_xSpin->value(), m_ySpin->value());
    m_currentItem->setSize(QSizeF(m_wSpin->value(), m_hSpin->value()));
    m_currentItem->setShapeType((ShapeType)m_shapeCombo->currentData().toInt());
}

void InspectorPanel::clearInspector() {
    m_titleLabel->setText("Inspector (No Selection)");
    m_currentItem = nullptr;
}