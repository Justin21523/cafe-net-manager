#pragma once
#include <QWidget>
class FloorPlanItem;
class QFormLayout;
class QLabel;
class QSpinBox;
class QComboBox;

class InspectorPanel : public QWidget {
    Q_OBJECT
public:
    explicit InspectorPanel(QWidget *parent = nullptr);
    void setItem(FloorPlanItem *item);
    void clearInspector();

private slots:
    void handleGeometryChanged();
    void handleSpinBoxChanged();

private:
    void buildUI(FloorPlanItem *item);
    void updateUIFromItem();

    QFormLayout *m_formLayout;
    QLabel *m_titleLabel;
    FloorPlanItem *m_currentItem = nullptr;
    
    QSpinBox *m_xSpin, *m_ySpin, *m_wSpin, *m_hSpin;
    QComboBox *m_shapeCombo;
    bool m_isUpdating = false; // Prevent infinite loop
};