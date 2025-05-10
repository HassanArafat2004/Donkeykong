#include "player.h"
#include "platform.h"
#include "ladder.h"
#include "barrel.h"
#include "mainwindow.h"
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include <QDebug>
#include <QTimer>

Player::Player() {
    leftAnimation = new QMovie(":/images/Move_left.gif");
    rightAnimation = new QMovie(":/images/Move_right.gif");
    
    leftAnimation->setScaledSize(QSize(30, 45));
    rightAnimation->setScaledSize(QSize(30, 45));
    
    connect(leftAnimation, &QMovie::frameChanged, this, &Player::updateFrame);
    connect(rightAnimation, &QMovie::frameChanged, this, &Player::updateFrame);
    
    upSprite = QPixmap(":/images/PlayerUp.png").scaled(30, 45);
    downSprite = QPixmap(":/images/PlayerDown.png").scaled(30, 45);
    idleSprite = QPixmap(":/images/Player.png").scaled(30, 45);
    
    setPixmap(idleSprite);
    currentDirection = Direction::IDLE;
    
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
    
    movingLeft = false;
    movingRight = false;
    climbingUp = false;
    climbingDown = false;
    jumping = false;
    onLadder = false;
    canClimb = false;
    onPlatform = false;
    wantsToJump = false;
    invincible = false;
    gameOver = false;
    lives = 3;
    score = 0;
    
    verticalSpeed = 0;
    horizontalSpeed = 0;
    gravity = 0.25;
    jumpVelocity = -5;
    maxSpeed = 4;
    acceleration = 0.3;
    deceleration = 0.2;
    
    setPos(100, 520);
    
    movementTimer = new QTimer(this);
    connect(movementTimer, &QTimer::timeout, this, &Player::updateMovement);
    movementTimer->start(16);

    invincibilityTimer = new QTimer(this);
    connect(invincibilityTimer, &QTimer::timeout, this, &Player::deactivateInvincibility);
}

Player::~Player() {
    if (movementTimer) {
        movementTimer->stop();
        delete movementTimer;
    }
    if (invincibilityTimer) {
        invincibilityTimer->stop();
        delete invincibilityTimer;
    }

    if (leftAnimation) {
        leftAnimation->stop();
        delete leftAnimation;
    }
    if (rightAnimation) {
        rightAnimation->stop();
        delete rightAnimation;
    }
}

void Player::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Left:
        movingLeft = true;
        break;
    case Qt::Key_Right:
        movingRight = true;
        break;
    case Qt::Key_Up:
        if (canClimb) {
            onLadder = true;
            climbingUp = true;
            verticalSpeed = 0;
        }
        break;
    case Qt::Key_Down:
        if (canClimb) {
            onLadder = true;
            climbingDown = true;
            verticalSpeed = 0;
        }
        break;
    case Qt::Key_Space:
        if (!jumping && !onLadder && onPlatform) {
            jumping = true;
            verticalSpeed = jumpVelocity;
        }
        break;
    }
}

void Player::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Left:
        movingLeft = false;
        break;
    case Qt::Key_Right:
        movingRight = false;
        break;
    case Qt::Key_Up:
        climbingUp = false;
        if (!climbingDown) {
            onLadder = false;
        }
        break;
    case Qt::Key_Down:
        climbingDown = false;
        if (!climbingUp) {
            onLadder = false;
        }
        break;
    }
}

void Player::updateFrame() {
    if (currentDirection == Direction::LEFT && leftAnimation->state() == QMovie::Running) {
        setPixmap(leftAnimation->currentPixmap());
    } else if (currentDirection == Direction::RIGHT && rightAnimation->state() == QMovie::Running) {
        setPixmap(rightAnimation->currentPixmap());
    }
}

void Player::updateSprite() {
    if (onLadder) {
        leftAnimation->stop();
        rightAnimation->stop();
        
        if (climbingUp) {
            currentDirection = Direction::UP;
            setPixmap(upSprite);
        } else if (climbingDown) {
            currentDirection = Direction::DOWN;
            setPixmap(downSprite);
        }
    } else if (movingLeft) {
        currentDirection = Direction::LEFT;
        rightAnimation->stop();
        leftAnimation->start();
    } else if (movingRight) {
        currentDirection = Direction::RIGHT;
        leftAnimation->stop();
        rightAnimation->start();
    } else if (jumping) {
        leftAnimation->stop();
        rightAnimation->stop();
        
        if (currentDirection == Direction::LEFT) {
            setPixmap(leftAnimation->currentPixmap());
        } else {
            setPixmap(rightAnimation->currentPixmap());
        }
    } else {
        leftAnimation->stop();
        rightAnimation->stop();
        
        currentDirection = Direction::IDLE;
        setPixmap(idleSprite);
    }
}

void Player::updateMovement() {
    if (gameOver) {
        return;
    }
    
    canClimb = checkLadderCollision();
    
    if (onLadder) {
        handleLadderMovement();
    } else {
        handleHorizontalMovement();
        handleVerticalMovement();
        applyGravity();
        if (!invincible) {
            checkBarrelJump();
        }
    }
    
    limitMovement();
    checkArtifactCollision();
    updateSprite();
}

void Player::handleHorizontalMovement() {
    if (movingLeft) {
        horizontalSpeed = qMax(-maxSpeed, horizontalSpeed - acceleration);
    } else if (movingRight) {
        horizontalSpeed = qMin(maxSpeed, horizontalSpeed + acceleration);
    } else {
        if (horizontalSpeed > 0) {
            horizontalSpeed = qMax(0.0, horizontalSpeed - deceleration);
        } else if (horizontalSpeed < 0) {
            horizontalSpeed = qMin(0.0, horizontalSpeed + deceleration);
        }
    }
    
    setX(x() + horizontalSpeed);
}

void Player::handleVerticalMovement() {
    if (jumping) {
        setY(y() + verticalSpeed);
        verticalSpeed += gravity;
        
        if (verticalSpeed > 0 && checkPlatformCollision()) {
            jumping = false;
            verticalSpeed = 0;
            onPlatform = true;
        }
    }
}

void Player::handleLadderMovement() {
    if (climbingUp) {
        setY(y() - 3);
    } else if (climbingDown) {
        setY(y() + 3);
    }
    
    QList<QGraphicsItem *> ladderItems = scene()->items(QRectF(
        x(), y(), boundingRect().width(), boundingRect().height()));
    for (QGraphicsItem *item : ladderItems) {
        Ladder *ladder = dynamic_cast<Ladder *>(item);
        if (ladder) {
            qreal ladderCenterX = ladder->x() + ladder->boundingRect().width() / 2;
            qreal playerWidth = boundingRect().width();
            setX(ladderCenterX - playerWidth / 2);
            break;
        }
    }
}

bool Player::checkLadderCollision() {
    QList<QGraphicsItem *> collisions = scene()->items(QRectF(
        x() - 5, y(), boundingRect().width() + 10, boundingRect().height()));
    
    for (QGraphicsItem *item : collisions) {
        if (dynamic_cast<Ladder *>(item)) {
            return true;
        }
    }
    return false;
}

bool Player::checkPlatformCollision() {
    QList<QGraphicsItem *> itemsBelow = scene()->items(QRectF(
        x(), y() + boundingRect().height(),
        boundingRect().width(), 5));
    
    for (QGraphicsItem *item : itemsBelow) {
        if (dynamic_cast<Platform *>(item)) {
            setY(item->y() - boundingRect().height());
            onPlatform = true;
            return true;
        }
    }
    onPlatform = false;
    return false;
}

void Player::applyGravity() {
    if (!jumping && !onLadder) {
        verticalSpeed += gravity;
        setY(y() + verticalSpeed);
        
        if (checkPlatformCollision()) {
            verticalSpeed = 0;
        }
    }
}

void Player::limitMovement() {
    if (x() < 0) setX(0);
    if (x() + boundingRect().width() > scene()->width()) {
        setX(scene()->width() - boundingRect().width());
    }
    if (y() < 0) setY(0);
    if (y() + boundingRect().height() > scene()->height()) {
        setY(scene()->height() - boundingRect().height());
        verticalSpeed = 0;
        jumping = false;
    }
}

void Player::resetPosition() {
    if (gameOver) return;
    
    setPos(50, 625);
    
    movingLeft = false;
    movingRight = false;
    climbingUp = false;
    climbingDown = false;
    jumping = false;
    onLadder = false;
    canClimb = false;
    onPlatform = false;
    wantsToJump = false;
    verticalSpeed = 0;
    horizontalSpeed = 0;
    
    jumpedBarrels.clear();
    
    if (invincible) {
        deactivateInvincibility();
    }
    
    QTimer::singleShot(0, this, &Player::reduceLives);
}

void Player::reduceLives() {
    if (lives > 0 && !gameOver) {
        lives--;
        emit livesChanged(lives);
        
        if (lives <= 0) {
            gameOver = true;
            
            movingLeft = false;
            movingRight = false;
            climbingUp = false;
            climbingDown = false;
            jumping = false;
            onLadder = false;
            
            if (leftAnimation) {
                leftAnimation->stop();
            }
            if (rightAnimation) {
                rightAnimation->stop();
            }
                
            if (movementTimer) {
                movementTimer->stop();
            }
                
            setOpacity(0.5);
            
            QTimer::singleShot(50, this, &Player::playerDied);
        }
    }
}

void Player::addScore(int points) {
    score += points;
    emit scoreChanged(score);
}

bool Player::checkBarrelJump() {
    if (!jumping) return false;

    QList<QGraphicsItem *> itemsBelow = scene()->items(QRectF(
        x() - 20,
        y() + boundingRect().height(),
        boundingRect().width() + 40,
        20));
    
    for (QGraphicsItem *item : itemsBelow) {
        Barrel *barrel = dynamic_cast<Barrel *>(item);
        if (barrel && !jumpedBarrels.contains(barrel)) {
            jumpedBarrels.insert(barrel);
            addScore(10);
            return true;
        }
    }
    return false;
}

void Player::activateInvincibility() {
    invincible = true;
    setOpacity(0.5);
    invincibilityTimer->start(5000);
}

void Player::deactivateInvincibility() {
    invincible = false;
    setOpacity(1.0);
}

void Player::checkArtifactCollision() {
    QList<QGraphicsItem *> collidingItems = scene()->items(QRectF(
        x(), y(), boundingRect().width(), boundingRect().height()));
    
    for (QGraphicsItem *item : collidingItems) {
        Artifact *artifact = dynamic_cast<Artifact *>(item);
        if (artifact && !artifact->isCollected()) {
            artifact->collect();
        }
    }
}
