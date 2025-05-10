#include "specialbarrel.h"
#include <QGraphicsScene>
#include <QRandomGenerator>

SpecialBarrel::SpecialBarrel() : Barrel() {
    // Set a different visual appearance
    setPixmap(QPixmap(":/images/barrel.png").scaled(30, 30).transformed(
        QTransform().rotate(45))); // Rotated barrel image to distinguish it

    // Different coloration could be applied with:
    // QPixmap pixmap = QPixmap(":/images/barrel.png").scaled(30, 30);
    // QPainter painter(&pixmap);
    // painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    // painter.fillRect(pixmap.rect(), QColor(255, 100, 100, 255)); // Reddish tint
    // setPixmap(pixmap);

    // Initialize special properties
    speed = 3;       // Faster than regular barrels
    bounce = false;  // Off by default
    zigzag = false;  // Off by default
    zigzagCounter = 0;

    // Make the default special barrel faster
    dx = dx * 1.5; // Increase horizontal speed
}

void SpecialBarrel::move() {
    // If zigzag mode is on, make the barrel zigzag
    if (zigzag && !falling) {
        zigzagCounter++;
        if (zigzagCounter > 10) {
            dx = -dx; // Change direction every 10 frames
            zigzagCounter = 0;
        }
    }

    // Apply current speed to position
    setPos(x() + dx, y() + dy);

    QList<QGraphicsItem *> collisions = collidingItems();
    bool touchingPlatform = false;

    for (QGraphicsItem *item : collisions) {
        if (dynamic_cast<Platform *>(item)) {
            touchingPlatform = true;
            break;
        }
    }

    if (touchingPlatform) {
        if (falling) {
            // If barrel just landed from falling
            if (bounce) {
                // If bounce is enabled, bounce up slightly and keep same direction
                dy = -4; // Jump up (bounce)
                falling = true; // Still in the air
            } else {
                // Regular behavior - change direction
                dx = -dx;
                falling = false;
            }
        } else {
            dy = 0; // No more falling, roll on platform
        }
    } else {
        dy = 3; // Falling faster than normal barrels
        falling = true;
    }

    // Randomly change direction sometimes for unpredictability
    if (!falling && QRandomGenerator::global()->bounded(100) < 2) { // 2% chance
        dx = -dx;
    }

    // If out of screen, delete
    if (y() > 600 || x() < -50 || x() > 850) {
        scene()->removeItem(this);
        delete this;
    }
}

void SpecialBarrel::setSpeed(int newSpeed) {
    speed = newSpeed;
    dx = (dx > 0) ? speed : -speed; // Maintain direction but change magnitude
}

int SpecialBarrel::getSpeed() const {
    return speed;
}

void SpecialBarrel::setBounce(bool newBounce) {
    bounce = newBounce;
}

bool SpecialBarrel::isBouncing() const {
    return bounce;
}

void SpecialBarrel::setZigZag(bool newZigzag) {
    zigzag = newZigzag;
}

bool SpecialBarrel::isZigZagging() const {
    return zigzag;
}
