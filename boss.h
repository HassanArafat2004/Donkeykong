#ifndef BOSS_H
#define BOSS_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QMovie>

class Boss : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    Boss();
    ~Boss();

private slots:
    void updateFrame();

private:
    QMovie* animation;
};

#endif
