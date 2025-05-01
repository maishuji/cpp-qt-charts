#ifndef DRAGGABLELINE_HPP
#define DRAGGABLELINE_HPP
#include <QGraphicsLineItem>

class DraggableLine : public QGraphicsLineItem {
 public:
    DraggableLine(qreal x1, qreal y1, qreal x2, qreal y2) :
        QGraphicsLineItem(x1, y1, x2, y2) {
        setFlags(QGraphicsItem::ItemIsMovable |
                 QGraphicsItem::ItemSendsGeometryChanges);
        setPen(QPen(Qt::red, 2, Qt::DashLine));
    }

 protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override {
        if (change == ItemPositionChange) {
            QPointF newPos = value.toPointF();
            // Lock movement to Y-axis only (horizontal line)
            return QPointF(pos().x(), newPos.y());
        }
        return QGraphicsLineItem::itemChange(change, value);
    }
};

#endif  // DRAGGABLELINE_HPP