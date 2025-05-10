#ifndef LADDER_H
#define LADDER_H

#include <QGraphicsPixmapItem>

class Ladder : public QGraphicsPixmapItem {
public:
    Ladder(qreal x, qreal y, qreal width, qreal height);
};

#endif
