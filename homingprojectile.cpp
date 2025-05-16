#include "homingprojectile.h"
#include "player.h"
#include <QGraphicsScene>
#include <QtMath>
#include <QPixmap>
#include <QPainter>

HomingProjectile::HomingProjectile(const QPointF& targetPos, QObject* parent)
    : QObject(parent), targetPosition(targetPos), speed(5.0)
{
    // Load and set the projectile image
    QPixmap projectileImage(":/images/fireball.png");
    if (!projectileImage.isNull()) {
        setPixmap(projectileImage.scaled(30, 30));
    } else {
        // Fallback if image not found - create a red circle
        QPixmap fallback(30, 30);
        fallback.fill(Qt::transparent);
        QPainter painter(&fallback);
        painter.setBrush(QBrush(Qt::red));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(0, 0, 30, 30);
        setPixmap(fallback);
    }

    // Set up movement timer
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &HomingProjectile::move);
    moveTimer->start(30); // Update every 30ms

    // Set initial rotation
    updateRotation();
}

HomingProjectile::~HomingProjectile()
{
    moveTimer->stop();
    delete moveTimer;
}

void HomingProjectile::move()
{
    if (!scene()) return;

    // Calculate direction vector to target
    QPointF direction = targetPosition - pos();
    qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());

    // If we're very close to the target, remove the projectile
    if (length < 5) {
        scene()->removeItem(this);
        deleteLater();
        return;
    }

    // Normalize direction and apply speed
    direction = direction / length * speed;

    // Move projectile
    setPos(x() + direction.x(), y() + direction.y());

    // Check for collision with player
    QList<QGraphicsItem*> collisions = scene()->items(QRectF(pos(), boundingRect().size()));
    for (QGraphicsItem* item : collisions) {
        Player* player = dynamic_cast<Player*>(item);
        if (player && !player->isInvincible()) {
            player->resetPosition();
            scene()->removeItem(this);
            deleteLater();
            return;
        }
    }

    // Remove if off screen
    if (x() < 0 || x() > scene()->width() || y() < 0 || y() > scene()->height()) {
        scene()->removeItem(this);
        deleteLater();
    }
}

void HomingProjectile::updateRotation()
{
    QPointF direction = targetPosition - pos();
    qreal angle = qAtan2(direction.y(), direction.x());
    setRotation(angle * 180 / M_PI);
} 