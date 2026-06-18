#pragma once

#include "graphics/FloorPlanItem.h"

class EnvironmentItem : public FloorPlanItem {
    Q_OBJECT

public:
    explicit EnvironmentItem(ObjectType type, QGraphicsItem *parent = nullptr);

    ObjectType objectType() const override { return m_type; }
    QString displayName() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    ObjectType m_type;
};
