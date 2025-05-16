#ifndef HOMINGPROJECTILE_H
#define HOMINGPROJECTILE_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPointF>
#include <QTimer>

class HomingProjectile : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    HomingProjectile(const QPointF& targetPos, QObject* parent = nullptr);
    ~HomingProjectile();

private slots:
    void move();

private:
    QPointF targetPosition;
    qreal speed;
    QTimer* moveTimer;
    void updateRotation();
};

#endif // HOMINGPROJECTILE_H 