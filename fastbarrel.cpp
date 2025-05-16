#include "fastbarrel.h"
#include "player.h"    // Add include for Player class
#include "platform.h"  // Add include for Platform class
#include <QGraphicsScene>
#include <QTimer>
#include <QList>

FastBarrel::FastBarrel() : Barrel() {
    // Increase the speed by modifying the dx and dz values
    dx = 10;  // Faster horizontal speed (original was 7)
    dz = 4;   // Faster falling speed (original was 2)
}

void FastBarrel::move() {
    // Don't process movement if the barrel is being deleted
    if (!scene()) return;
    
    // Check for player collision
    QList<QGraphicsItem *> collisions = scene()->items(QRectF(
        x(), y(), boundingRect().width(), boundingRect().height()));
    
    for (QGraphicsItem *item : collisions) {
        Player *player = dynamic_cast<Player *>(item);
        if (player) {
            if (!player->isInvincible()) {
                // Schedule player reset and barrel deletion for next frame
                QTimer::singleShot(0, player, &Player::resetPosition);
                QTimer::singleShot(50, this, [this]() {
                    if (scene()) {
                        scene()->removeItem(this);
                        deleteLater();
                    }
                });
                return;  // Exit early to prevent further movement
            }
        }
    }

    // Check platform beneath
    QList<QGraphicsItem *> below = scene()->items(QPointF(x() + boundingRect().width() / 2, y() + boundingRect().height() + 1));
    bool onPlatform = false;
    
    for (QGraphicsItem *item : below) {
        if (dynamic_cast<Platform *>(item)) {
            setY(item->y() - boundingRect().height());
            onPlatform = true;

            // If we were falling and just landed on a platform
            if (falling) {
                int platformLevel = static_cast<int>(item->y() / 100);
                dx = (platformLevel % 2 == 0) ? 6 : -6;  // Faster platform movement
                falling = false;
            }
            break;
        }
    }

    if (!onPlatform) {
        falling = true;
        // When falling, continue horizontal movement for gliding effect
        setX(x() + dx * 0.8); // Slightly faster horizontal movement during fall
        setY(y() + dz);       // Vertical movement
    } else {
        updatePlatformLevel();

        // Move horizontally on platform
        setX(x() + dx);

        // Check if barrel should fall or bounce based on platform level
        switch (currentPlatformY) {
            case 175: // Top platform
                if (dx > 0 && x() >= 550) { // Fall on right side
                    falling = true;
                } else if (dx < 0 && x() <= 350) {
                    dx = -dx; // Bounce on left side
                }
                break;
                
            case 300: // Fourth platform - Roll left to right
                if (dx > 0 && x() >= 800) { // Fall on right side
                    falling = true;
                } else if (dx < 0 && x() <= 250) {
                    dx = -dx; // Bounce on left side
                }
                break;
                
            case 425: // Third platform - Roll right to left
                if (dx < 0 && x() <= 150) { // Fall on left side
                    falling = true;
                } else if (dx > 0 && x() >= 700) {
                    dx = -dx; // Bounce on right side
                }
                break;
                
            case 550: // Second platform - Roll left to right
                if (dx > 0 && x() >= 800) { // Fall on right side
                    falling = true;
                } else if (dx < 0 && x() <= 250) {
                    dx = -dx; // Bounce on left side
                }
                break;
                
            case 675: // Bottom platform - Roll right to left
                if (dx < 0 && x() <= 100) { // Fall on left side
                    falling = true;
                } else if (dx > 0 && x() >= 850) {
                    dx = -dx; // Bounce on right side
                }
                break;
                
            case 800: // Ground - Roll to right and despawn
                if (dx > 0 && x() >= 950) {
                    scene()->removeItem(this);
                    deleteLater();
                    return;
                }
                break;
        }
    }

    // Update animation state based on movement
    updateState();
    
    // Remove barrel if it goes off screen
    if (y() > scene()->height() + 50 || x() < -50 || x() > scene()->width() + 50) {
        scene()->removeItem(this);
        deleteLater();
        return;
    }
} 