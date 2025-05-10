#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QTimer>
#include <QSet>
#include <QMovie>
#include "platform.h"
#include "ladder.h"
#include "barrel.h"
#include "artifact.h"

class Player : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    Player();
    ~Player();

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resetPosition();
    void reduceLives();
    int getLives() const { return lives; }
    int getScore() const { return score; }
    void addScore(int points);
    void activateInvincibility();
    bool isInvincible() const { return invincible; }
    bool isGameOver() const { return gameOver; }

signals:
    void livesChanged(int newLives);
    void scoreChanged(int newScore);
    void playerDied();

private slots:
    void updateMovement();
    void deactivateInvincibility();
    void updateFrame();

private:
    bool movingLeft;
    bool movingRight;
    bool climbingUp;
    bool climbingDown;
    bool jumping;
    bool onLadder;
    bool canClimb;
    bool onPlatform;
    bool wantsToJump;
    bool invincible;
    bool gameOver;
    int lives;
    int score;
    QSet<Barrel*> jumpedBarrels;
    qreal verticalSpeed;
    qreal horizontalSpeed;
    qreal gravity;
    qreal jumpVelocity;
    qreal maxSpeed;
    qreal acceleration;
    qreal deceleration;
    QTimer *movementTimer;
    QTimer *invincibilityTimer;
    
    enum class Direction { LEFT, RIGHT, UP, DOWN, IDLE };
    Direction currentDirection;
    QMovie *leftAnimation;
    QMovie *rightAnimation;
    QPixmap upSprite;
    QPixmap downSprite;
    QPixmap idleSprite;
    
    void handleHorizontalMovement();
    void handleVerticalMovement();
    void handleLadderMovement();
    bool checkLadderCollision();
    bool checkPlatformCollision();
    bool checkBarrelJump();
    void checkArtifactCollision();
    void applyGravity();
    void limitMovement();
    void updateSprite();
};

#endif
