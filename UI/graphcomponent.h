#ifndef GRAPHCOMPONENT_H
#define GRAPHCOMPONENT_H

#include <QGraphicsItem>
#include <QPainter>
#include <QWidget>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

class GraphComponent : public QGraphicsItem
{
public:
    GraphComponent(int id, const QString& type);
    virtual ~GraphComponent() override = default;

    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override = 0;

    int getId() const { return m_id; }
    QString getType() const { return m_type; }

    void updatePhysicsData(double voltage, double current);

    QPointF getLeftConnectionPoint() const;
    QPointF getRightConnectionPoint() const;
    QPointF getConnectionPointScene(int port) const;

    int getSelectedPort() const { return m_selectedPort; }
    void setSelectedPort(int port) { m_selectedPort = port; }

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    int m_id;
    QString m_type;
    double m_voltage;
    double m_current;
    int m_selectedPort;
};

#endif // GRAPHCOMPONENT_H