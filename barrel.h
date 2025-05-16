#ifndef BARREL_H
#define BARREL_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QMovie>

class Barrel : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    Barrel();
    ~Barrel();

public slots:
    virtual void move();
    void updateAnimation();

protected:
    int dx;
    int dy;
    double dz;
    bool falling;
    bool fallTriggered;
    int currentPlatformY;
    void updatePlatformLevel();
    void updateState();

private:
    enum class State {
        RollLeft,
        RollRight,
        FallLeft,
        FallRight
    };
    State currentState;

    QMovie *rollLeftAnimation;
    QMovie *rollRightAnimation;
    QMovie *fallLeftAnimation;
    QMovie *fallRightAnimation;
};

#endif // BARREL_H
