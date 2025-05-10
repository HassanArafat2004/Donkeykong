#include "ladder.h"
#include <QPixmap>

Ladder::Ladder(qreal x, qreal y, qreal width, qreal height) {
    setPixmap(QPixmap(":/images/ladder.png").scaled(width, height));
    setPos(x, y);
}
