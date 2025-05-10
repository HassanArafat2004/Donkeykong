#include "barrel.h"
#include "platform.h"
#include "player.h"
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include <QList>

// Define constant size for barrel
const int BARREL_SIZE = 30;

Barrel::Barrel() {
    // Initialize animations
    rollLeftAnimation = new QMovie(":/images/barrel.png");
    rollRightAnimation = new QMovie(":/images/barrel.png");
    fallLeftAnimation = new QMovie(":/images/fallleft.gif");
    fallRightAnimation = new QMovie(":/images/fallright.gif");

    // Scale all animations to the same size
    QSize barrelSize(BARREL_SIZE, BARREL_SIZE);
    rollLeftAnimation->setScaledSize(barrelSize);
    rollRightAnimation->setScaledSize(barrelSize);
    fallLeftAnimation->setScaledSize(barrelSize);
    fallRightAnimation->setScaledSize(barrelSize);

    // Set initial pixmap with correct size
    setPixmap(QPixmap(":/images/barrel.png").scaled(BARREL_SIZE, BARREL_SIZE));

    // Connect animation frame updates
    connect(rollLeftAnimation, &QMovie::frameChanged, this, &Barrel::updateAnimation);
    connect(rollRightAnimation, &QMovie::frameChanged, this, &Barrel::updateAnimation);
    connect(fallLeftAnimation, &QMovie::frameChanged, this, &Barrel::updateAnimation);
    connect(fallRightAnimation, &QMovie::frameChanged, this, &Barrel::updateAnimation);

    // Start with rolling right
    currentState = State::RollRight;
    rollRightAnimation->start();

    // Initialize movement parameters
    dx = 7;
    dy = 3;
    dz = 2;
    falling = true;
    fallTriggered = false;
    currentPlatformY = -1;

    // Set up movement timer
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Barrel::move);
    timer->start(30);
}

Barrel::~Barrel() {
    delete rollLeftAnimation;
    delete rollRightAnimation;
    delete fallLeftAnimation;
    delete fallRightAnimation;
}

void Barrel::updateAnimation() {
    QMovie *currentAnimation = nullptr;
    switch (currentState) {
        case State::RollLeft:
            currentAnimation = rollLeftAnimation;
            break;
        case State::RollRight:
            currentAnimation = rollRightAnimation;
            break;
        case State::FallLeft:
            currentAnimation = fallLeftAnimation;
            break;
        case State::FallRight:
            currentAnimation = fallRightAnimation;
            break;
    }

    if (currentAnimation) {
        setPixmap(currentAnimation->currentPixmap());
    }
}

void Barrel::updateState() {
    State newState;
    
    if (falling) {
        // When falling, maintain left/right direction but switch to falling animation
        newState = (dx < 0) ? State::FallLeft : State::FallRight;
    } else {
        // When rolling, use roll animation based on direction
        newState = (dx < 0) ? State::RollLeft : State::RollRight;
    }

    if (newState != currentState) {
        // Stop current animation
        switch (currentState) {
            case State::RollLeft:
                rollLeftAnimation->stop();
                break;
            case State::RollRight:
                rollRightAnimation->stop();
                break;
            case State::FallLeft:
                fallLeftAnimation->stop();
                break;
            case State::FallRight:
                fallRightAnimation->stop();
                break;
        }

        // Start new animation
        currentState = newState;
        switch (currentState) {
            case State::RollLeft:
                rollLeftAnimation->start();
                break;
            case State::RollRight:
                rollRightAnimation->start();
                break;
            case State::FallLeft:
                fallLeftAnimation->start();
                break;
            case State::FallRight:
                fallRightAnimation->start();
                break;
        }
    }
}

void Barrel::updatePlatformLevel() {
    int yPos = static_cast<int>(y());
    if (yPos >= 90 && yPos < 130) currentPlatformY = 100;
    else if (yPos >= 190 && yPos < 230) currentPlatformY = 200;
    else if (yPos >= 290 && yPos < 330) currentPlatformY = 300;
    else if (yPos >= 390 && yPos < 430) currentPlatformY = 400;
    else if (yPos >= 490 && yPos < 530) currentPlatformY = 500;
    else currentPlatformY = -1;
}

void Barrel::move() {
    // Don't process movement if the barrel is being deleted
    if (!scene()) return;
    
    // Check for player collision
    QList<QGraphicsItem *> collisions = scene()->items(QRectF(
        x(), y(), boundingRect().width(), boundingRect().height()));
    
    for (QGraphicsItem *item : collisions) {
        Player *player = dynamic_cast<Player *>(item);
        if (player && !player->isGameOver()) {
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
                dx = (platformLevel % 2 == 0) ? 4 : -4;
                falling = false;
            }
            break;
        }
    }

    if (!onPlatform) {
        falling = true;
        // When falling, continue horizontal movement for gliding effect
        setX(x() + dx * 0.7); // Slower horizontal movement during fall
        setY(y() + dz);       // Vertical movement
    } else {
        // Update platform level
        updatePlatformLevel();

        // Move horizontally on platform
        setX(x() + dx);

        // Check if barrel should fall or bounce
        if (currentPlatformY == 100 || currentPlatformY == 300 || currentPlatformY == 500) {
            // Odd-numbered platforms: move right, fall from right edge
            if (dx > 0 && x() >= 780) {
                falling = true;
            } else if (dx < 0 && x() <= 200) {
                dx = -dx; // Bounce from left edge
            }
        } else if (currentPlatformY == 200 || currentPlatformY == 400) {
            // Even-numbered platforms: move left, fall from left edge
            if (dx < 0 && x() <= 120) {
                falling = true;
            } else if (dx > 0 && x() >= 700) {
                dx = -dx; // Bounce from right edge
            }
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
